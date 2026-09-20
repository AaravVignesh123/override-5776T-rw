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
