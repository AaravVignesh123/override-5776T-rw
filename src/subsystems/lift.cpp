#include "main.h"
#include "subsystems/lift.hpp"
#include "autonomous/odometry.hpp"


pros::Motor liftMotor1(11);
pros::Motor liftMotor2(20);

void liftUp() {
    setMotorSpeed(liftMotor1, -3.0_x);
    setMotorSpeed(liftMotor2, 3.0_x);
}
void liftDown() {
    setMotorSpeed(liftMotor1, 3.0_x);
    setMotorSpeed(liftMotor2, -3.0_x);
}
