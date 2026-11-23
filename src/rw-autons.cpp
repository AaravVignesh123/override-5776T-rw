#include "rw-config.h"
#include <cstdio>

// IMPORTANT: Remember to add respective function declarations to include/rw-config.h
// Then add each routine to the auton selector with addAuton() in runPreAutonomous() (src/rw-user.cpp)
//
// Converted from the LemLib routes. How each call was translated:
//   moveToPoint(x, y, fwd, min, max, t)       -> moveToPoint(fx(x), fy(y), fwd ? 1 : -1, t, true, max * 12)
//   moveToPose(x, y, h, fwd, lead, min, max, t)-> boomerang(fx(x), fy(y), fwd ? 1 : -1, h, lead, t, true, max * 12)
//   turnToAngle(h, min, max, t)               -> turnToAngle(h, t, true, max * 12)
//   relative(d, min, max, t)                  -> driveTo(d, t, true, max * 12)   (holds heading with its own PID)
//   setChassisPose(x, y, h)                   -> startAt(x, y, h)
//   odomDistanceX.correctPosition(...)        -> distanceReset(resetPositionX, "x")
//   async = true                              -> runAsync([]{ ... });  then waitForMotion() before the next move
// Speeds: LemLib 0-127 (1_x = 127) became volts (12 = full). Default timeouts were 1000 ms (moves) and
// 800 ms (turns), and LemLib minSpeed only made a motion exit early, so it's dropped.
// All lengths are inches (1 ft = 12").

// ============================================================================
// LOCAL -> FIELD COORDINATES
// ============================================================================
// Each route sets where its local (0, 0) is on the field. The origins below are worked out from the
// old setOriginPosition() values, moved 1.75" because OdomDistance put the walls at 72" and RW uses
// the real 70.25". The old code gave each sensor its own origin, and they disagreed by up to ~6",
// so these are estimates: MEASURE each starting spot on the field (tape from two walls to the
// robot's center) and put the real numbers here. Until then distance_reset_max_jump (6") makes RW
// skip a reset that disagrees too much instead of jumping the position.

static double origin_x = 0, origin_y = 0;

void setLocalOrigin(double field_x, double field_y) {
  origin_x = field_x;
  origin_y = field_y;
}
double fx(double local_x) { return origin_x + local_x; }
double fy(double local_y) { return origin_y + local_y; }

void startAt(double local_x, double local_y, double heading_deg) {
  setPose(fx(local_x), fy(local_y), heading_deg);
}

// RW picks the wall from the robot's position and heading (the old ForceToWall is no longer needed).
// It returns false and leaves the position alone if the beam is too steep, near a corner, sees nothing,
// or would move the robot more than distance_reset_max_jump. This logs both cases to the terminal.
bool distanceReset(bool (*reset)(), const char* name) {
  const double old_x = x_pos, old_y = y_pos;
  const bool ok = reset();
  if (ok) {
    printf("[reset %s] (%.1f, %.1f) -> (%.1f, %.1f)\n", name, old_x, old_y, x_pos, y_pos);
  } else {
    printf("[reset %s] skipped at (%.1f, %.1f, %.1f deg)\n", name, x_pos, y_pos, getInertialHeading());
  }
  return ok;
}

// ============================================================================
// HELPERS
// ============================================================================

// Runs the lift up or down for a set time, then stops it
void liftingLift(liftUP liftDirection, uint32_t timeWantedMsec, uint32_t delayMsec) {
  const uint32_t startTime = pros::millis();
  while (pros::millis() - startTime < timeWantedMsec) {
    if (liftDirection == LIFT_UP) {
      liftUp();
    } else {
      liftDown();
    }
    pros::delay(delayMsec);
  }
  liftStop();
}

// NEEDS TO BE TUNED
// Grab from the right matchloader and score in R_SIDE_ALLIANCE_GOAL, L_SIDE_ALLIANCE_GOAL or MIDDLE_GOAL.
// Uses the current route's local origin (skills)
void matchloadingRight(float objectHeight, whichGoal goal) {
  moveToPoint(fx(69), fy(14), -1, 1500, true, 8.4);
  pros::delay(150);
  turnToAngle(180, 800);
  distanceReset(resetPositionLeft, "left");    // right wall (old: LEFT_RIGHT)
  distanceReset(resetPositionFront, "front");  // back wall (old: FRONT_BACK)
  pros::delay(150);
  moveToPoint(fx(70), fy(-6), 1, 1500, true, 9.6);  // to the matchloader
  pros::delay(150);
  closeClaw();
  pros::delay(150);
  liftToPosition(0.5, 127);  // up a little to clear the matchloader
  driveTo(-12, 1000);
  liftToPosition(2, 100);

  if (goal == R_SIDE_ALLIANCE_GOAL) {
    liftToPosition(objectHeight, 127);
    turnToAngle(-90, 800);
    pros::delay(75);
    moveToPoint(fx(30), fy(9), 1, 1500, true, 6.6);
    pros::delay(75);
    distanceReset(resetPositionLeft, "left");  // back wall (old: FRONT_BACK)
    liftToPosition(1.5, 127);
    pros::delay(75);
    openClaw();  // release pin
  } else if (goal == L_SIDE_ALLIANCE_GOAL) {
    liftToPosition(objectHeight, 127);
    turnToAngle(-150, 800);
    pros::delay(125);
    moveToPoint(fx(58.25), fy(32.5), 1, 2000, true, 6.6);
    pros::delay(100);
    // Beam is 30 deg off the far wall here, right at distance_reset_max_angle, so this one may be skipped
    distanceReset(resetPositionBack, "back");
    liftToPosition(0, 127);
    pros::delay(250);
    openClaw();
  } else if (goal == MIDDLE_GOAL) {
    moveToPoint(fx(69), fy(58), -1, 2000, true, 7.8);
    pros::delay(50);
    turnToAngle(-90, 800);
    driveTo(-14, 1000);
    toggleOnce();
    pros::delay(200);
    runAsync([]{ moveToPoint(fx(0), fy(62), 1, 2000, true, 6.6); });
    liftToPosition(objectHeight, 110);  // correct height for the pin
    pros::delay(50);
    distanceReset(resetPositionBack, "back");
    openClaw();
    waitForMotion();
  }
}

// Driver macros. They start from wherever the robot is (the old versions set the pose to (0, 0) first)
static double macro_x = 0, macro_y = 0;

void farSideMatchloader() {
  macro_x = x_pos;
  macro_y = y_pos;
  driveTo(-3, 1000);
  boomerang(macro_x + 15.6, macro_y - 16.44, -1, 180, 0.4, 1000);
  turnToAngle(-90, 800);
  boomerang(macro_x + 18.24, macro_y - 31.2, 1, -90, 0.4, 1000);
}

void scoreFarSide(int pinStack) {
  // Assumes the robot faces 180 when the macro starts, like before
  setPose(x_pos, y_pos, 180);
  macro_x = x_pos;
  macro_y = y_pos;
  boomerang(macro_x + 15.6, macro_y - 16.44, -1, 180, 0.4, 1000);
  turnToAngle(0, 800);
  runAsync([]{ boomerang(macro_x - 18.24, macro_y - 34.2, 1, 0, 0.4, 1000); });
  liftToPosition(pinStack * 4, 100);
  openClaw();
  waitForMotion();  // new: driver control waits for the move to finish instead of fighting it
}

// ============================================================================
// LEFT
// ============================================================================

// The standing-pin part of the left route was commented out in the LemLib code. It's converted here
// and compiles, but only runs when this is true
constexpr bool LEFT_RUN_STANDING_PINS = false;

static void leftStandingPins(bool awp, bool noPreload) {
  turnToAngle(-39, 800);

  // First standing pin
  moveToPoint(fx(-24), fy(29.5), 1, 2000, true, 7.2);
  moveToPoint(fx(-25), fy(32), 1, 2000, true, 4.8);
  pros::delay(30);
  closeClaw();
  liftToPosition(1, 127);
  driveTo(4, 1000);
  pros::delay(180);

  // Score it in the alliance goal
  liftToPosition(8, 127);
  turnToAngle(177, 800);
  pros::delay(150);
  moveToPoint(fx(-29.5), fy(19.5), 1, 2000, true, 4.8);
  pros::delay(175);
  liftToPosition(3, 127);
  pros::delay(225);
  distanceReset(resetPositionFront, "front");
  openClaw();

  // Second standing pin
  moveToPoint(fx(-30.5), fy(26.5), -1, 2000, true, 6);
  driveTo(-5, 1000);
  turnToAngle(-142, 800);
  pros::delay(125);
  liftToPosition(-2, 127);
  moveToPoint(fx(-44.5), fy(13), 1, 2000, true, 7.2);
  moveToPoint(fx(-49.5), fy(18), 1, 2000, true, 6);
  pros::delay(75);
  closeClaw();
  driveTo(2, 1000);
  tareLiftPosition();

  if (awp) {
    // Alliance's goal for AWP
    liftToPosition(7, 100);
    turnToAngle(0, 800);
    moveToPoint(fx(-57), fy(44), 1, 1500, true, 6);
    liftToPosition(0, 110);
    pros::delay(300);
    openClaw();
  } else if (noPreload) {
    // Our black goal
    turnToAngle(25, 800);
    liftToPosition(1, 127);
    boomerang(fx(-23.5), fy(28), 1, 120, 0.4, 1500, true, 9.6);
    runAsync([]{ boomerang(fx(17), fy(12), 1, 90, 0.4, 1500, true, 9.6); });
    liftToPosition(8, 127);
    waitForMotion();
    moveToPoint(fx(21), fy(12), 1, 1500, true, 4.8);
    openClaw();
    moveToPoint(fx(0), fy(9), 1, 1500, true, 7.2);
    turnToAngle(0, 800);
    moveToPoint(fx(0), fy(32), 1, 1500, true, 4.8);
    closeClaw();
    runAsync([]{ moveToPoint(fx(0), fy(40), 1, 1500, true, 4.8); });
    liftToPosition(8, 127);
    liftToPosition(0, 127);
    openClaw();
    waitForMotion();
  } else {
    // Our alliance goal for elims
    liftToPosition(12, 127);
    turnToAngle(90, 800);
    pros::delay(175);
    moveToPoint(fx(-31), fy(14), 1, 1500, true, 6);
    liftToPosition(6, 100);
    pros::delay(175);
    openClaw();
    driveTo(-5, 1000);
    liftToPosition(0, 127);
  }
}

void LeftAuto(bool awp, bool noPreload) {
  // Old origins: front (5, -12), left (-4, -14), back (-3, -13.5). This route resets Y with the
  // left sensor on the back wall, so that one is used
  setLocalOrigin(-4, -14.25);
  startAt(-2.4, 2.04, -44);

  toggleOnce();
  pros::delay(700);
  liftToPosition(3, 127);

  // Alliance goal: score preload
  boomerang(fx(-18), fy(11.3), 1, -75, 0.65, 2000, true, 8.4);
  boomerang(fx(-29), fy(13.5), 1, -90, 0.65, 2000, true, 5.04);
  pros::delay(215);
  liftToPosition(0, 100);
  pros::delay(225);
  openClaw();
  distanceReset(resetPositionLeft, "left");  // back wall (old: FRONT_BACK)

  moveToPoint(fx(-17), fy(12), -1, 1500, true, 4.8);

  if (LEFT_RUN_STANDING_PINS) leftStandingPins(awp, noPreload);
}

// ============================================================================
// RIGHT
// ============================================================================

void RightAuto() {
  // Same start as skills (see skills() for the origin)
  setLocalOrigin(-8.75, 9.5);
  startAt(-2.16, 0.72, 44);

  toggleOnce();
  pros::delay(510);
  liftToPosition(3, 127);

  // Alliance goal: score preload
  boomerang(fx(13), fy(13), 1, 75, 0.55, 2000, true, 7.8);
  boomerang(fx(23), fy(15), 1, 90, 0.65, 2000, true, 5.04);
  pros::delay(180);
  liftToPosition(0, 100);
  pros::delay(185);
  openClaw();
  distanceReset(resetPositionLeft, "left");  // far wall (old: FRONT_BACK)

  moveToPoint(fx(18), fy(14), -1, 1500, true, 6);
  turnToAngle(39, 800);

  // First standing pin
  moveToPoint(fx(15.5), fy(25.5), 1, 2000, true, 7.2);
  moveToPoint(fx(19.5), fy(32.5), 1, 2000, true, 5.28);
  pros::delay(25);
  tareLiftPosition();
  closeClaw();
  liftToPosition(1, 127);
  driveTo(4, 1000);
  pros::delay(180);

  // Score it in the alliance goal
  liftToPosition(8, 127);
  turnToAngle(180, 800);
  pros::delay(150);
  moveToPoint(fx(25), fy(19), 1, 2000, true, 6);
  pros::delay(175);
  liftToPosition(3, 127);
  pros::delay(225);
  distanceReset(resetPositionFront, "front");  // back wall (old: FRONT_BACK)
  openClaw();

  // Second standing pin
  driveTo(-6.5, 1000);
  turnToAngle(135, 800);
  pros::delay(125);
  liftToPosition(-1, 127);
  moveToPoint(fx(41.5), fy(18), 1, 2000, true, 5.4);
  pros::delay(75);
  closeClaw();
  driveTo(2, 1000);

  // Score it in the alliance goal (end of the four pin auto)
  liftToPosition(13, 127);
  turnToAngle(90, 800);
  pros::delay(175);
  boomerang(fx(24), fy(14), 1, 90, 0.65, 2000, true, 5.04);
  liftToPosition(8, 100);
  liftToPosition(0, 127);
  pros::delay(175);
  openClaw();
  driveTo(-10, 1000);
}

void RightAwp() {
  // Old origins: front (4, -10), left (-8, -12), back (0, -18). Y is reset with the front sensor on
  // the back wall, so that one is used
  setLocalOrigin(4, -8.25);
  startAt(-0.2832, -14.2992, 44.56);

  toggleOnce();
  pros::delay(175);
  liftToPosition(3.5, 100);

  // Alliance goal: score preload
  boomerang(fx(31), fy(-1), 1, 90, 0.58, 1500, true, 4.8);
  pros::delay(250);
  liftToPosition(0, 100);
  pros::delay(250);
  openClaw();
  pros::delay(150);

  // Standing pin #1
  driveTo(-5, 1000);
  turnToAngle(38, 800);
  moveToPoint(fx(20), fy(14), 1, 1500, true, 7.2);
  pros::delay(75);
  closeClaw();
  driveTo(2, 1000);

  liftToPosition(8, 100);
  turnToAngle(180, 800);
  pros::delay(150);
  moveToPoint(fx(32), fy(2), 1, 2000, true, 4.8);
  pros::delay(175);
  liftToPosition(3, 100);
  pros::delay(250);
  distanceReset(resetPositionFront, "front");  // back wall (old: FRONT_BACK)
  openClaw();

  // Standing pin #2
  driveTo(-7, 1000);
  turnToAngle(142, 800);
  pros::delay(250);
  liftToPosition(0, 127);
  moveToPoint(fx(46.5), fy(2), 1, 1500, true, 7.2);
  moveToPoint(fx(48.5), fy(1), 1, 1500, true, 6);
  pros::delay(25);
  closeClaw();
  driveTo(2, 1000);

  // Our alliance goal
  liftToPosition(14, 127);
  turnToAngle(0, 800);
  pros::delay(175);
  boomerang(fx(49), fy(0), 1, 0, 0.4, 2000, true, 6);
  liftToPosition(12, 100);
  pros::delay(1000);
  openClaw();
}

// ============================================================================
// SOLO AWP
// ============================================================================

void SoloAWP() {
  // No distance resets in this route yet, so it has no origin. Measure the start and set it before
  // adding resets or turning on MCL
  setLocalOrigin(0, 0);
  startAt(0, -13.2, 180);

  driveTo(-6, 1000);
  moveToPoint(fx(0), fy(-20.2), 1, 1000);
  driveTo(-6, 1000);
  moveToPoint(fx(0), fy(-22.2), 1, 1000);

  moveToPoint(fx(0), fy(1), -1, 1500, true, 8.4);  // right alliance goal
  liftingLift(LIFT_UP, 300);
  scoringReadyPosition();
  turnToAngle(-90, 800);
  boomerang(fx(22.2), fy(3), -1, -90, 0.5, 1500);  // blue goal
  pros::delay(650);
  liftingLift(LIFT_DOWN, 400);
  storePin();
  moveToPoint(fx(22.2), fy(8), -1, 500, true, 7.2);
  turnToAngle(-45, 800);
  boomerang(fx(45.6), fy(3), -1, -90, 0.5, 2000, true, 8.4);
  pros::delay(650);
  liftUp();  // the LemLib version never stopped the lift after these 0.4 s, so it keeps running up
  pros::delay(400);
  driveTo(-5, 1000);
  turnToAngle(-135, 800);
  boomerang(fx(24), fy(-12), -1, -90, 0.4, 500);  // between the alliance goal and the wall
  clawTwistMaxPosition();
  boomerang(fx(0), fy(22), -1, -90, 0.4, 1500);  // at the cup
  storePin();
  restPosition();
  scoringReadyPosition();
  pros::delay(650);
  liftUp();  // not stopped afterwards (same as before)
  pros::delay(400);
  turnToAngle(90, 800);
  boomerang(fx(27), fy(3), -1, -90, 0.5, 1500);  // alliance color goal
  pros::delay(650);
  liftingLift(LIFT_DOWN, 400);
  storePin();  // pin and cup dropped in the alliance goal
  turnToAngle(0, 800);
  boomerang(fx(24), fy(24), -1, 90, 0.4, 1500);  // at the pin and cup
  storePin();
  restPosition();
  scoringReadyPosition();
  pros::delay(650);
  liftUp();  // not stopped afterwards (same as before)
  pros::delay(400);
  turnToAngle(135, 800);  // facing the black goal
  boomerang(fx(-24), fy(3), -1, 90, 0.4, 1500);
  pros::delay(650);
  liftingLift(LIFT_DOWN, 400);
  storePin();
}

// ============================================================================
// SKILLS
// ============================================================================

void skills() {
  // Old origins: front (6, 7), left (-7, 12), back (-6, 9).
  // X: only the left sensor resets X (on the right wall, in matchloadingRight), so -7 - 1.75.
  // Y: resets on the back wall said ~8.75-13.75 and on the far wall ~7.25-10.25, so 9.5 splits them
  setLocalOrigin(-8.75, 9.5);
  startAt(-2.16, 0.72, 44);

  toggleOnce();
  pros::delay(510);
  liftToPosition(3, 127);

  // Alliance goal: score preload
  boomerang(fx(14), fy(9), 1, 75, 0.5, 2000, true, 7.8);
  boomerang(fx(24), fy(12), 1, 90, 0.65, 2000, true, 5.04);
  pros::delay(180);
  liftToPosition(0, 100);
  pros::delay(185);
  openClaw();

  driveTo(-8, 1000);
  turnToAngle(39.25, 800);

  // First standing pin
  moveToPoint(fx(17.2), fy(26.45), 1, 2000, true, 7.2);
  moveToPoint(fx(21.2), fy(29.45), 1, 2000, true, 4.8);
  pros::delay(25);
  closeClaw();
  liftToPosition(1, 127);
  driveTo(4, 1000);
  pros::delay(180);

  // Score it in the alliance goal
  liftToPosition(8, 127);
  turnToAngle(180, 800);
  pros::delay(150);
  moveToPoint(fx(26), fy(19), 1, 2000, true, 4.8);
  pros::delay(175);
  liftToPosition(3, 127);
  pros::delay(225);
  distanceReset(resetPositionFront, "front");  // back wall (old: FRONT_BACK)
  openClaw();

  // Second standing pin
  driveTo(-6.5, 1000);
  turnToAngle(131, 800);
  pros::delay(100);
  liftToPosition(-1, 127);
  moveToPoint(fx(44), fy(18), 1, 2000, true, 5.4);
  pros::delay(75);
  closeClaw();
  driveTo(2, 1000);
  liftToPosition(2, 127);
  turnToAngle(90, 800);
  liftToPosition(13, 127);
  pros::delay(125);
  distanceReset(resetPositionLeft, "left");  // far wall (old: FRONT_BACK)

  // Score it in the alliance goal
  boomerang(fx(24), fy(12), 1, 90, 0.65, 2000, true, 5.04);
  liftToPosition(8, 100);
  pros::delay(250);
  openClaw();
  driveTo(-2, 1000);
  liftToPosition(-4, 127);

  // Third and fourth pins, right alliance goal
  matchloadingRight(18, R_SIDE_ALLIANCE_GOAL);
  distanceReset(resetPositionLeft, "left");
  liftToPosition(-7, 127);
  matchloadingRight(23, R_SIDE_ALLIANCE_GOAL);
  distanceReset(resetPositionLeft, "left");

  // Left alliance goal: pins and cups
  liftToPosition(-9, 127);
  matchloadingRight(2, L_SIDE_ALLIANCE_GOAL);
  liftToPosition(-10, 127);
  matchloadingRight(7, L_SIDE_ALLIANCE_GOAL);
  liftToPosition(-12, 127);
  matchloadingRight(14, L_SIDE_ALLIANCE_GOAL);
  liftToPosition(-14, 127);
  matchloadingRight(18, L_SIDE_ALLIANCE_GOAL);

  // Middle goal pin
  liftToPosition(-16, 127);
  matchloadingRight(14, MIDDLE_GOAL);
}

// ============================================================================
// TUNING AND TESTS
// ============================================================================

// RW's tuning auton: tune distance_* and turn_* with this (docs: Guide > Tuning)
void exampleAuton() {
  driveTo(60, 3000);
  turnToAngle(90, 2000);
  turnToAngle(135, 2000);
  turnToAngle(150, 2000);
  turnToAngle(160, 2000);
  turnToAngle(165, 2000);
  turnToAngle(0, 2000);
  driveTo(-60, 3000);
}

// RW's odometry check: the robot should end where it started
void exampleAuton2() {
  moveToPoint(24, 24, 1, 2000, false);
  moveToPoint(48, 48, 1, 2000, true);
  moveToPoint(24, 24, -1, 2000, true);
  moveToPoint(0, 0, 1, 2000, true);
  correct_angle = 0;
  driveTo(24, 2000, false, 8);
  turnToAngle(90, 800, false);
  turnToAngle(180, 800, true);
}

static void showResult(const char* label, double value) {
  char line[20];
  snprintf(line, sizeof(line), "%s %.2f   ", label, value);
  printf("%s\n", line);
  controller_1.set_text(0, 0, line);
  pros::delay(2000);
}

// One 90 deg turn; shows how far off it ended
void turnTuning() {
  setPose(0, 0, 0);
  const double startAngle = getInertialHeading();
  turnToAngle(90, 800);
  showResult("Turn err", getInertialHeading() - startAngle - 90);
}

// One 24" drive; shows how far off it ended
void translationTuning() {
  setPose(0, 0, 0);
  const double startY = y_pos;
  driveTo(24, 1000);
  showResult("Drive err", y_pos - startY - 24);
}

// Old testingAuton: put the robot's center 48" from the left and back walls (tape measure), facing
// the far wall, and check what each distance reset says
void distanceResetTest() {
  setPose(-70.25 + 48, -70.25 + 48, 0);
  pros::delay(500);
  distanceReset(resetPositionBack, "back");
  distanceReset(resetPositionLeft, "left");
  distanceReset(resetPositionFront, "front");
  printPositionDebug();
}
