#include "main.h"
#include "subsystems/claw.hpp"
#include "autonomous/odometry.hpp"

pros::Motor clawMotor(-1);

void storePin() {
    setMotorSpeed(clawMotor, -2.0_x);
}
void releasePin() {
    setMotorSpeed(clawMotor, 2.0_x);
}
