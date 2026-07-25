#include "main.h"
#include "subsystems/intake.hpp"
#include "autonomous/odometry.hpp"
#include "subsystems/pneumatics.hpp"

// Temporary stubbed-out intake subsystem for odometry testing.
pros::Motor intakeBottomLarge(-1);
pros::Motor intakeBottomSmall(-1);
pros::Motor intakeTop(-1);

void storeBalls() {
    // Intake disabled for odometry testing.
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
}
