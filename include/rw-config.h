#pragma once
#include "api.h"
#include "rw/api.h"

// ============================================================================
// 5776T devices (besides the ones RW uses, which are in rw/devices.h).
// All of them are defined in src/rw-config.cpp, so the whole port map is in one place.
// ============================================================================
extern pros::Motor clawMotor;
extern pros::Motor clawTwistMotor;
extern pros::Motor liftMotor1;
extern pros::Motor liftMotor2;
extern pros::Motor rightToggleMotor;
extern pros::Motor intakeBottomLarge;
extern pros::Optical toggleColorSensor;
extern pros::Distance clawDistanceSensor;
extern pros::Imu tipImu;
extern pros::adi::DigitalOut clawPiston;

// Subsystem functions
#include "subsystems/claw.hpp"
#include "subsystems/intake.hpp"
#include "subsystems/lift.hpp"
#include "subsystems/pneumatics.hpp"
#include "subsystems/toggle.hpp"

// ============================================================================
// Helpers (src/rw-user.cpp)
// ============================================================================
extern uint32_t autonStartTime;   // pros::millis() when the auton started
bool isOverheating();             // true if any drive motor is over temperature
void printPositionDebug();        // show the pose on the controller and freeze the auton
double secondsSinceAutonStart();

// ============================================================================
// Auton helpers (src/rw-autons.cpp)
// The routes were tuned in LemLib-style local coordinates (the robot starts near (0, 0)).
// RW's distance resets and MCL need field coordinates (field center = (0, 0), walls at +/-70.25").
// setLocalOrigin() says where local (0, 0) is on the field, and fx()/fy() convert a local
// position to a field one, so the tuned numbers keep working with field-centered odometry.
// ============================================================================
void setLocalOrigin(double field_x, double field_y);
double fx(double local_x);
double fy(double local_y);
void startAt(double local_x, double local_y, double heading_deg);  // setPose() in local coordinates
bool distanceReset(bool (*reset)(), const char* name);              // runs a reset and logs it if it was skipped

enum liftUP { LIFT_UP, LIFT_DOWN };
enum whichGoal { R_SIDE_ALLIANCE_GOAL, L_SIDE_ALLIANCE_GOAL, MIDDLE_GOAL };

void matchloadingRight(float objectHeight, whichGoal goal = R_SIDE_ALLIANCE_GOAL);
void liftingLift(liftUP liftDirection, uint32_t timeWantedMsec, uint32_t delayMsec = 100);
void farSideMatchloader();   // driver macro (A + R2)
void scoreFarSide(int pinStack);  // driver macro (A + Up/Down/R1)

// ============================================================================
// Autonomous routines (src/rw-autons.cpp). Add each one to the selector in src/rw-user.cpp
// ============================================================================
void LeftAuto(bool awp, bool noPreload);
void RightAuto();
void RightAwp();
void SoloAWP();
void skills();
void turnTuning();
void translationTuning();
void distanceResetTest();
void exampleAuton();
void exampleAuton2();

// Called from src/main.cpp (src/rw-user.cpp)
void runPreAutonomous();
void runAutonomous();
void runDriver();
