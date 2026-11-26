#include "rw-config.h"
#include "rw/hardware.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <string>

// Pre-auton, autonomous and driver code. src/main.cpp calls these.

// ============================================================================
// HELPERS
// ============================================================================

uint32_t autonStartTime = 0;

double secondsSinceAutonStart() { return (pros::millis() - autonStartTime) / 1000.0; }

bool isOverheating() {
  for (int i = 0; i < 3; i++) {
    if (left_chassis.is_over_temp(i) == 1 || right_chassis.is_over_temp(i) == 1) return true;
  }
  return false;
}

// Shows the pose on the controller (and the terminal), stops everything and freezes the auton.
// Same job as the old printPositionDebug(), reading RW's x_pos / y_pos.
void printPositionDebug() {
  cancelMotion();
  stopChassis(pros::MotorBrake::hold);
  stopIntake();
  liftStop();
  char line[20];
  printf("Position debug: (%.2f, %.2f, %.2f deg) at %.2fs\n", x_pos, y_pos, getInertialHeading(), secondsSinceAutonStart());
  controller_1.clear();
  pros::delay(60);
  snprintf(line, sizeof(line), "X %.1f Y %.1f", x_pos, y_pos);
  controller_1.set_text(0, 0, line);
  pros::delay(60);
  snprintf(line, sizeof(line), "H %.1f", getInertialHeading());
  controller_1.set_text(1, 0, line);
  pros::delay(60);
  snprintf(line, sizeof(line), "T %.2fs", secondsSinceAutonStart());
  controller_1.set_text(2, 0, line);
  while (true) pros::delay(1000);
}

// ============================================================================
// AUTONOMOUS
// ============================================================================

void runAutonomous() {
  tareLiftPosition();
  autonStartTime = pros::millis();
  // Runs the routine picked on the Brain screen (or with the controller arrows while disabled).
  // Add routines to the list in runPreAutonomous() below.
  runSelectedAuton();
}

// ============================================================================
// DRIVER CONTROL
// ============================================================================

// Controller sticks read -127 to 127; RW's driver functions take -100 to 100
double stick(pros::controller_analog_e_t axis) {
  return controller_1.get_analog(axis) * 100.0 / 127.0;
}

// Button edges (pressed / released this loop), which PROS and RW don't give directly
struct ControllerButton {
  bool pressed = false;
  bool held = false;
  bool released = false;
  bool wasHeld = false;
};

static void pollControllerDigital(ControllerButton& button, pros::controller_digital_e_t buttonCode) {
  button.held = controller_1.get_digital(buttonCode);
  button.pressed = button.held && !button.wasHeld;
  button.released = !button.held && button.wasHeld;
  button.wasHeld = button.held;
}

static ControllerButton xButton, yButton, aButton, bButton, upButton, downButton, leftButton, rightButton,
    l1Button, l2Button, r1Button, r2Button;

// Anti-tip. Off by default: the tipping IMU was on port -7, which PROS can't use, so this code never
// actually ran on the robot. Check tipImu's port and the sign of the correction before turning it on.
// When it runs it replaces the throttle stick with a correction, until the last 30 seconds of driver.
constexpr bool ANTI_TIP_ENABLED = false;
static double tipAngle = 0.0;
static double lastTipAngles[10] = {};  // mean filter of the last 10 angles for noise reduction
constexpr int kTipSamples = sizeof(lastTipAngles) / sizeof(lastTipAngles[0]);
static double antiTipCommand = 0.0;

static double arrMean(const double arr[], int length) {
  double sum = 0;
  for (int i = 0; i < length; i++) sum += arr[i];
  return sum / length;
}

// Driver state, shown on the controller screen
static bool clawToggle = false;   // false = closed (LOW), true = open (HIGH)
static bool macroActive = false;
static bool enableMacros = false;
static int showDebug = 0;
constexpr int kDebugPages = 4;
static uint32_t driverStartTime = 0;

struct RecordedPose {
  double x;
  double y;
  double heading;
};

constexpr int kMaxRecordedWaypoints = 400;
static RecordedPose recordedWaypoints[kMaxRecordedWaypoints];
static std::atomic<int> recordedWaypointCount{0};
static std::atomic<bool> routeRecording{false};
static bool routeRecordingFull = false;
static uint32_t routeRecordingStartTime = 0;
static uint32_t lastRouteSampleTime = 0;
static double routeEndHeading = 0;

static void recordRoutePose(bool force = false) {
  const uint32_t now = pros::millis();
  if (!force && now - lastRouteSampleTime < 100) return;
  lastRouteSampleTime = now;

  const RecordedPose pose{x_pos, y_pos, getInertialHeading()};
  const int count = recordedWaypointCount.load();
  if (count > 0) {
    const double distance = std::hypot(pose.x - recordedWaypoints[count - 1].x,
                                       pose.y - recordedWaypoints[count - 1].y);
    if (distance < (force ? 0.25 : 2.0)) {
      routeEndHeading = pose.heading;
      return;
    }
  }

  routeEndHeading = pose.heading;
  if (count >= kMaxRecordedWaypoints) {
    if (!routeRecordingFull) {
      routeRecordingFull = true;
      printf("Route recorder reached its %d-waypoint limit; remaining movement won't be recorded.\n",
             kMaxRecordedWaypoints);
    }
    return;
  }

  recordedWaypoints[count] = pose;
  recordedWaypointCount.store(count + 1);
}

static void exportRecordedRoute() {
  const int count = recordedWaypointCount.load();
  if (count < 2) {
    printf("Route export skipped: drive at least 0.25 inches before stopping the recording.\n");
    return;
  }

  std::string code;
  code.reserve(256 + count * 64);
  char line[160];
  bool codeValid = true;
  auto append = [&](const char* format, auto... args) {
    if (!codeValid) return;
    const int written = snprintf(line, sizeof(line), format, args...);
    if (written < 0 || written >= static_cast<int>(sizeof(line))) {
      codeValid = false;
      return;
    }
    code.append(line, written);
  };

  append("// Paste into src/rw-autons.cpp; declare recordedRoute in include/rw-config.h\n");
  append("// Then add addAuton(\"Recorded\", recordedRoute) in runPreAutonomous().\n");
  append("void recordedRoute() {\n");
  append("  setPose(%.2f, %.2f, %.2f);\n", recordedWaypoints[0].x, recordedWaypoints[0].y,
         recordedWaypoints[0].heading);
  append("  const Path recordedPath = pathFromPoints({\n");
  for (int i = 0; i < count; i++) {
    append("    Waypoint(%.2f, %.2f, 0.60),\n", recordedWaypoints[i].x, recordedWaypoints[i].y);
  }
  append("  });\n");
  const uint32_t timeout = std::max<uint32_t>(5000, (pros::millis() - routeRecordingStartTime) * 2 + 2000);
  append("  followPath(recordedPath, %u);\n", timeout);
  append("  turnToAngle(%.2f, 2000);\n", routeEndHeading);
  append("}\n");

  if (!codeValid) {
    printf("Route export failed: generated code exceeded a formatting limit.\n");
    return;
  }

  if (hw::sdSave("rw-recorded-route.cpp", code.c_str(), static_cast<int>(code.size()))) {
    printf("Route code saved to /usd/rw-recorded-route.cpp (%d waypoints).\n", count);
  } else {
    printf("No writable microSD found; copy this generated route from the PROS terminal:\n%s\n", code.c_str());
  }
}

static void toggleRouteRecording() {
  if (!routeRecording.load()) {
    recordedWaypointCount.store(0);
    routeRecordingFull = false;
    routeRecordingStartTime = pros::millis();
    lastRouteSampleTime = routeRecordingStartTime;
    routeRecording.store(true);
    recordRoutePose(true);
    if (!pros::usd::is_installed()) {
      printf("Route recording started; no microSD detected, so the generated code will print to the PROS terminal.\n");
    } else {
      printf("Route recording started. Press D-pad Left again to stop and export.\n");
    }
    return;
  }

  recordRoutePose(true);
  routeRecording.store(false);
  exportRecordedRoute();
}

// The old LLEMU debug pages, moved to the controller screen (3 lines) because pros::lcd draws
// over RW's selector. X cycles pages. The controller takes one line every 50 msec, so lines rotate.
static void controllerDebugTask() {
  int line = 0;
  int lastPage = -1;
  char text[20];
  while (true) {
    if (showDebug != lastPage) {
      controller_1.clear();
      lastPage = showDebug;
      pros::delay(60);
      continue;
    }
    text[0] = '\0';
    if (routeRecording.load()) {
      if (line == 0) snprintf(text, sizeof(text), "REC %3d waypoints ", recordedWaypointCount.load());
      if (line == 1) snprintf(text, sizeof(text), "D-pad Left: stop  ");
      if (line == 2) snprintf(text, sizeof(text), "Drive the route   ");
    } else {
      switch (showDebug) {
        case 0:  // pose
          if (line == 0) snprintf(text, sizeof(text), "X%6.1f Y%6.1f ", x_pos, y_pos);
          if (line == 1) snprintf(text, sizeof(text), "H%6.1f        ", getInertialHeading());
          if (line == 2) snprintf(text, sizeof(text), "T%4.0fs %s     ", (pros::millis() - driverStartTime) / 1000.0, isOverheating() ? "HOT" : "");
          break;
        case 1:  // distance sensors (inches, claw in mm)
          if (line == 0) snprintf(text, sizeof(text), "F%5.1f L%5.1f  ", front_sensor.get() / 25.4, left_sensor.get() / 25.4);
          if (line == 1) snprintf(text, sizeof(text), "B%5.1f C%4dmm ", back_sensor.get() / 25.4, getClawDistanceMm());
          if (line == 2) snprintf(text, sizeof(text), "Hue %5.0f      ", toggleColorSensor.get_hue());
          break;
        case 2:  // mechanisms
          if (line == 0) snprintf(text, sizeof(text), "Claw %s Mac %d ", clawToggle ? "OPEN" : "SHUT", enableMacros);
          if (line == 1) snprintf(text, sizeof(text), "Lift %6.0f     ", liftMotor2.get_position());
          if (line == 2) snprintf(text, sizeof(text), "Twist %6.0f    ", clawTwistMotor.get_position());
          break;
        case 3:  // drive health / anti-tip
          if (line == 0) snprintf(text, sizeof(text), "Tip %5.1f      ", tipAngle);
          if (line == 1) snprintf(text, sizeof(text), "Untip %5.1f    ", antiTipCommand);
          if (line == 2) snprintf(text, sizeof(text), "Hot drive: %d  ", isOverheating());
          break;
      }
    }
    controller_1.set_text(line, 0, text);
    line = (line + 1) % 3;
    pros::delay(60);
  }
}

void runDriver() {
  stopChassis(pros::MotorBrake::coast);
  heading_correction = false;
  driverStartTime = pros::millis();

  const double tipAngleOffset = tipImu.get_roll();
  for (int i = 0; i < kTipSamples; i++) lastTipAngles[i] = 0.0;

  clawToggle = false;
  static pros::Task debugTask(controllerDebugTask);

  while (true) {
    const double currentTimeSec = (pros::millis() - driverStartTime) / 1000.0;

    // Poll controller digital
    pollControllerDigital(xButton, pros::E_CONTROLLER_DIGITAL_X);
    pollControllerDigital(yButton, pros::E_CONTROLLER_DIGITAL_Y);
    pollControllerDigital(aButton, pros::E_CONTROLLER_DIGITAL_A);
    pollControllerDigital(bButton, pros::E_CONTROLLER_DIGITAL_B);
    pollControllerDigital(upButton, pros::E_CONTROLLER_DIGITAL_UP);
    pollControllerDigital(downButton, pros::E_CONTROLLER_DIGITAL_DOWN);
    pollControllerDigital(leftButton, pros::E_CONTROLLER_DIGITAL_LEFT);
    pollControllerDigital(rightButton, pros::E_CONTROLLER_DIGITAL_RIGHT);
    pollControllerDigital(l1Button, pros::E_CONTROLLER_DIGITAL_L1);
    pollControllerDigital(l2Button, pros::E_CONTROLLER_DIGITAL_L2);
    pollControllerDigital(r1Button, pros::E_CONTROLLER_DIGITAL_R1);
    pollControllerDigital(r2Button, pros::E_CONTROLLER_DIGITAL_R2);

    if (leftButton.pressed) toggleRouteRecording();

    // L2 runs the Brain-selected auton directly for standalone testing without a competition switch.
    if (l2Button.pressed) {
      if (routeRecording.load()) {
        printf("Stop route recording before running the selected auton.\n");
        continue;
      }
      debugTask.suspend();
      runSelectedAuton();
      startAutonSelector();
      debugTask.resume();
      pros::delay(10);
      continue;
    }

    // Macros (hold A). These drive with RW motions and block driver control until they finish
    enableMacros = aButton.held;
    /*
    if (enableMacros && r2Button.pressed) farSideMatchloader();
    if (enableMacros && upButton.pressed) scoreFarSide(2);
    if (enableMacros && downButton.pressed) scoreFarSide(4);
    if (enableMacros && r1Button.pressed) scoreFarSide(3);
    */

    // Claw controls
    if (yButton.pressed) {
      clawToggle = !clawToggle;
      liftToPosition(1, 127);
    }
    setClaw(clawToggle);

    // Toggle rollers (R1, or automatically when the optical sensor sees the other alliance's color)
    if (!enableMacros && (r1Button.held || isNonAllianceColorDetected())) {
      toggleRollers();
    } else {
      stopToggle();
    }

    // Auto-grab closes the claw once each time an object comes into range
    if (autoGrab()) {
      clawToggle = false;
      setClaw(clawToggle);
    }

    // Lift macro
    if (l1Button.held) {
      macroActive = true;
      liftToPosition(0, 127);
    }

    // Lift controls
    if (bButton.held) {
      liftDown();
      macroActive = false;
    } else if (rightButton.held) {
      liftUp();
      macroActive = false;
    } else if (!macroActive) {
      liftStop();
    }

    // Debug pages on the controller
    if (xButton.pressed) showDebug = (showDebug + 1) % kDebugPages;

    // Split arcade: left stick forward, right stick turn (turning scaled down like before)
    double throttle = stick(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    double turn = stick(pros::E_CONTROLLER_ANALOG_RIGHT_X) / 1.5;

    // Anti-tip protection
    if (ANTI_TIP_ENABLED) {
      tipAngle = std::remainder(tipImu.get_roll() - tipAngleOffset, 360.0);
      const double tipMagnitude = std::abs(tipAngle);
      const double tipSign = std::signbit(tipAngle) ? 1 : -1;
      const double tipVelocity = (tipAngle - arrMean(lastTipAngles, kTipSamples)) * tipSign * 10.0;

      // Disable anti-tip for the final 30 seconds of driver control
      if (std::isfinite(tipAngle) && tipMagnitude > 10 && currentTimeSec < 75) {
        const double correctionMagnitude =
            std::clamp((tipMagnitude - 10.0) * 2.0 + std::max(0.0, tipVelocity) * 0.1, 0.0, 80.0);
        antiTipCommand = tipSign * correctionMagnitude;     // LemLib power (-127 to 127)
        throttle = antiTipCommand * 100.0 / 127.0;          // RW stick scale (-100 to 100)
      } else {
        antiTipCommand = 0;
      }

      for (int i = kTipSamples - 1; i > 0; i--) lastTipAngles[i] = lastTipAngles[i - 1];
      lastTipAngles[0] = tipAngle;
    }

    driverArcade(throttle, turn);
    if (routeRecording.load()) recordRoutePose();
    // Tank instead: driverTank(stick(pros::E_CONTROLLER_ANALOG_LEFT_Y), stick(pros::E_CONTROLLER_ANALOG_RIGHT_Y));
    // Tune feel with driver_curve and driver_deadband in src/rw-config.cpp

    pros::delay(10);
  }
}

// ============================================================================
// PRE-AUTONOMOUS
// ============================================================================

void runPreAutonomous() {
  // Mechanism setup (from the old initialize()). Start with the claw twist at rest and the lift down
  clawTwistMotor.tare_position();
  clawTwistMotor.set_brake_mode(pros::MotorBrake::hold);
  tareLiftPosition();
  closeClaw();

  // Calibrate both IMUs at the same time (about 2 seconds)
  tipImu.reset(false);
  inertial_sensor.reset(true);
  for (int i = 0; i < 300 && tipImu.is_calibrating(); i++) pros::delay(10);

  const std::int32_t opticalInitResult = initializeToggleColorSensor();
  if (opticalInitResult != PROS_SUCCESS) {
    printf("Optical init failed: %ld\n", (long)opticalInitResult);
  }

  // odom tracking (picks the setup from using_horizontal_tracker / using_vertical_tracker)
  startOdometry();

  // Optional: MCL uses the distance sensors to keep odometry from drifting while driving.
  // Turn it on once every auton's setLocalOrigin() is measured (src/rw-autons.cpp)
  // startMcl();

  // Autonomous routines for the selector, in the order they appear on the Brain screen.
  // The selector remembers the last pick (the old code always ran skills)
  //addAuton("Skills", skills);
  addAuton("Right", RightAuto);
  //addAuton("Right AWP", RightAwp);
  addAuton("Left", []{ LeftAuto(false, false); });
  //addAuton("Solo AWP", SoloAWP);
  addAuton("Tuning Test", exampleAuton);
  addAuton("Odom Test", exampleAuton2);
  addAuton("Turn Check", turnTuning);
  addAuton("Drive Check", translationTuning);
  addAuton("Dist Reset Test", distanceResetTest);
  addAuton("IMU Scale", []{ measureImuScale(); });
  addAuton("Drive Calibration", calibrateDrive);  // For the simulator (see the docs: Simulator > Calibrating)
  startAutonSelector();
}
