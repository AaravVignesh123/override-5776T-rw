#include "main.h"
#include "rw-config.h"

// RW-Template runs the robot. Everything else lives in:
//   src/rw-config.cpp  ports and settings
//   src/rw-user.cpp    pre-auton setup, the auton list, and driver control
//   src/rw-autons.cpp  autonomous routes
// Don't use pros::lcd anywhere: it draws over RW's auton selector on the Brain screen.

void initialize() { runPreAutonomous(); }
void disabled() {}
void competition_initialize() {}
void autonomous() { runAutonomous(); }
void opcontrol() { runDriver(); }
