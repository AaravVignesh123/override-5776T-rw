#include "main.h"
#include "subsystems/claw.hpp"
#include "autonomous/odometry.hpp"

pros::Motor clawMotor(-10);
pros::Motor clawTwistMotor(7);
const double TWIST_UP_LIMIT = -400.0;
const double TWIST_START_POSITION = 0.0;
const double TWIST_SLOW_ZONE = 100.0;
const double TWIST_HOLD_GAIN = 0.01;
const double TWIST_MAX_HOLD_POWER = 0.25;

void holdTwistPosition(double targetPosition) {
    double positionError = targetPosition - clawTwistMotor.get_position();
    double holdPower = positionError * TWIST_HOLD_GAIN;

    if (holdPower > TWIST_MAX_HOLD_POWER) {
        holdPower = TWIST_MAX_HOLD_POWER;
    } else if (holdPower < -TWIST_MAX_HOLD_POWER) {
        holdPower = -TWIST_MAX_HOLD_POWER;
    }

    clawTwistMotor.set_brake_mode(pros::MotorBrake::hold);
    setMotorSpeed(clawTwistMotor, holdPower * motorPowerFactor);
}

void storePin() {
    setMotorSpeed(clawMotor, -127_power);
}
void releasePin() {
    setMotorSpeed(clawMotor, 127_power);
}
void scoringReady(){
    setMotorSpeed(clawTwistMotor, -1.0_x);
}
void scoringReady2(){
    double position = clawTwistMotor.get_position();
    if (position > TWIST_UP_LIMIT + TWIST_SLOW_ZONE) {
        setMotorSpeed(clawTwistMotor, -1.0_x);
    } else if (position > TWIST_UP_LIMIT) {
        setMotorSpeed(clawTwistMotor, -0.3_x);
    } else {
        holdTwistPosition(TWIST_UP_LIMIT);
    }
}
void rest(){
    setMotorSpeed(clawTwistMotor, 1.0_x);
}
void rest2(){
    if (clawTwistMotor.get_position() < TWIST_START_POSITION) {
        setMotorSpeed(clawTwistMotor, 1.0_x);
    } else {
        holdTwistPosition(TWIST_START_POSITION);
    }
}


// Draft position-control version for the claw twist.
// Assumes the motor axle and claw gear are approximately 1:1.
// The negative sign is based on the current setup, where negative motor
// encoder values move the claw toward scoring position.

const double TWIST_REST_POSITION = 0.0;
const double TWIST_SCORING_POSITION = -900.0;  // Adjust this value based on the actual scoring position
const int TWIST_POSITION_SPEED = 100;

// Run this once after the claw is physically placed in its rest position.
// clawTwistMotor.tare_position();

void scoringReadyPosition() {
    clawTwistMotor.set_brake_mode(pros::MotorBrake::hold);
    clawTwistMotor.move_absolute(TWIST_SCORING_POSITION,
                                 TWIST_POSITION_SPEED);
}

void restPosition() {
    clawTwistMotor.set_brake_mode(pros::MotorBrake::hold);
    clawTwistMotor.move_absolute(TWIST_REST_POSITION,
                                 TWIST_POSITION_SPEED);
}
/*
void scoringReadyPosition() {
    clawTwistMotor.move_absolute(-360.0, 100);
}

void restPosition() {
    clawTwistMotor.move_absolute(0.0, 100);
}
*/
