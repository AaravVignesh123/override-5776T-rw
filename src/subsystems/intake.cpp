#include "main.h"
#include "subsystems/intake.hpp"
#include "autonomous/odometry.hpp"
#include "subsystems/pneumatics.hpp"

// Temporary stubbed-out intake subsystem for odometry testing.
pros::Motor intakeBottomLarge(-5);

void intakePins() {
    setMotorSpeed(intakeBottomLarge, 1.5_x);
}
void outtakePins() {
    setMotorSpeed(intakeBottomLarge, -1.5_x);
}   

void scoreBallsUp() {
    // Intake disabled for odometry testing.
}

void scoreBallsMiddle(bool driverControl) {
    (void)driverControl;
    // Intake disabled for odometry testing.
}

void clearMiddleSection() {
    // Intake disabled for odometry testing.
}

void scoreBallsDown() {
    // Intake disabled for odometry testing.
}

void stopIntake() {
    // Intake disabled for odometry testing.
    setMotorSpeed(intakeBottomLarge, 0_x);
}
