#include "subsystems/chassis.hpp"
#include "autonomous/odometry.hpp"
#include "lemlib/api.hpp"
#include "toggle.hpp"
#include "main.h"

// change ports --> ports need to be defined
//pros::Motor leftToggleMotor(-7);
pros::Motor rightToggleMotor(8);

void toggleRollers() {
    setMotorSpeed(rightToggleMotor, 100_power);
}
void toggleOnce(){
    rightToggleMotor.move_relative(720, 200);
}