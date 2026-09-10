#include "main.h"
#include "subsystems/claw.hpp"
#include "autonomous/odometry.hpp"

pros::Motor clawMotor(-1);
pros::Motor clawTwistMotor(7);
void storePin() {
    setMotorSpeed(clawMotor, -2.0_x);
}
void releasePin() {
    setMotorSpeed(clawMotor, 2.0_x);
}
void scoringReady(){
    setMotorSpeed(clawTwistMotor, -1.0_x);
}
void rest(){
    setMotorSpeed(clawTwistMotor, 1.0_x);
}