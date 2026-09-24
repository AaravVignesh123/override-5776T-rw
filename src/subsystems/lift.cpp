#include "main.h"
#include "subsystems/lift.hpp"
#include "autonomous/odometry.hpp"


pros::Motor liftMotor1(11);
pros::Motor liftMotor2(20);

void liftUp() {
    setMotorSpeed(liftMotor1, -127_power);
    setMotorSpeed(liftMotor2, 127_power);
}
void liftDown() {
    setMotorSpeed(liftMotor1, 127_power);
    setMotorSpeed(liftMotor2, -127_power);
}

void liftToPosition(double position, int32_t velocity) {
    liftMotor1.set_brake_mode(pros::MotorBrake::hold);
    liftMotor2.set_brake_mode(pros::MotorBrake::hold);
    liftMotor1.move_absolute(position, velocity);
    liftMotor2.move_absolute(-position, velocity);
}

void tareLiftPosition() {
    liftMotor1.tare_position();
    liftMotor2.tare_position();
}
