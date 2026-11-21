#include "main.h"
#include "subsystems/lift.hpp"
#include "autonomous/odometry.hpp"
#include <cmath>


pros::Motor liftMotor1(11);
pros::Motor liftMotor2(20);
//change later, get inches lifted per motor revolution
constexpr double LIFT_INCHES_PER_MOTOR_REV = 3.2; 
constexpr double LIFT_POSITION_TOLERANCE_DEGREES = 4;

void liftUp() {
    setMotorSpeed(liftMotor1, -127_power);
    setMotorSpeed(liftMotor2, 127_power);
}
void liftDown() {
    setMotorSpeed(liftMotor1, 127_power);
    setMotorSpeed(liftMotor2, -127_power);
}

void liftToPosition(float position, int32_t velocity) {
    const double motorDegrees = position * 360.0 / LIFT_INCHES_PER_MOTOR_REV;
    liftMotor1.set_brake_mode(pros::MotorBrake::hold);
    liftMotor2.set_brake_mode(pros::MotorBrake::hold);
    liftMotor1.move_absolute(-motorDegrees, velocity);
    liftMotor2.move_absolute(motorDegrees, velocity);
}

void waitForLiftPosition(float position) {
    const double motorDegrees = position * 360.0 / LIFT_INCHES_PER_MOTOR_REV;
    while (std::abs(liftMotor1.get_position() + motorDegrees) > LIFT_POSITION_TOLERANCE_DEGREES ||
           std::abs(liftMotor2.get_position() - motorDegrees) > LIFT_POSITION_TOLERANCE_DEGREES) {
        pros::delay(10);
    }
}

void tareLiftPosition() {
    liftMotor1.tare_position();
    liftMotor2.tare_position();
}
