#include "rw-config.h"
#include <cmath>

// Change later: inches lifted per motor revolution
constexpr double LIFT_INCHES_PER_MOTOR_REV = 3.2;
constexpr double LIFT_POSITION_TOLERANCE_DEGREES = 4;

static double liftDegrees(float position) { return position * 360.0 / LIFT_INCHES_PER_MOTOR_REV; }

void liftUp() {
    liftMotor1.move(-127);
    liftMotor2.move(127);
}
void liftDown() {
    liftMotor1.move(127);
    liftMotor2.move(-127);
}
void liftStop() {
    liftMotor1.move(0);
    liftMotor2.move(0);
}

void liftToPosition(float position, int32_t velocity) {
    const double motorDegrees = liftDegrees(position);
    liftMotor1.set_brake_mode(pros::MotorBrake::hold);
    liftMotor2.set_brake_mode(pros::MotorBrake::hold);
    liftMotor1.move_absolute(-motorDegrees, velocity);
    liftMotor2.move_absolute(motorDegrees, velocity);
}

bool isLiftAtPosition(float position) {
    const double motorDegrees = liftDegrees(position);
    return std::abs(liftMotor1.get_position() + motorDegrees) <= LIFT_POSITION_TOLERANCE_DEGREES &&
           std::abs(liftMotor2.get_position() - motorDegrees) <= LIFT_POSITION_TOLERANCE_DEGREES;
}

// Now has a timeout, so a jammed lift can't hang the whole auton
void waitForLiftPosition(float position, uint32_t timeoutMsec) {
    const uint32_t start = pros::millis();
    while (!isLiftAtPosition(position) && pros::millis() - start < timeoutMsec) {
        pros::delay(10);
    }
}

void tareLiftPosition() {
    liftMotor1.tare_position();
    liftMotor2.tare_position();
}
