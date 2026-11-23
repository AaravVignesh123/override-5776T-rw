#include "rw-config.h"
#include <algorithm>

// Old speeds were LemLib power (-127 to 127, or 1_x = 127), so they stay on that scale with move()
const double TWIST_UP_LIMIT = -400.0;
const double TWIST_START_POSITION = 0.0;
const double TWIST_SLOW_ZONE = 100.0;
const double TWIST_HOLD_GAIN = 0.01;
const double TWIST_MAX_HOLD_POWER = 0.25;

void holdTwistPosition(double targetPosition) {
    double positionError = targetPosition - clawTwistMotor.get_position();
    double holdPower = std::clamp(positionError * TWIST_HOLD_GAIN, -TWIST_MAX_HOLD_POWER, TWIST_MAX_HOLD_POWER);

    clawTwistMotor.set_brake_mode(pros::MotorBrake::hold);
    clawTwistMotor.move(holdPower * 127);
}

void storePin() { clawMotor.move(-127); }
void releasePin() { clawMotor.move(127); }
void stopClawMotor() { clawMotor.move(0); }

void scoringReady() { clawTwistMotor.move(-127); }
void scoringReady2() {
    double position = clawTwistMotor.get_position();
    if (position > TWIST_UP_LIMIT + TWIST_SLOW_ZONE) {
        clawTwistMotor.move(-127);
    } else if (position > TWIST_UP_LIMIT) {
        clawTwistMotor.move(-0.3 * 127);
    } else {
        holdTwistPosition(TWIST_UP_LIMIT);
    }
}
void rest() { clawTwistMotor.move(127); }
void rest2() {
    if (clawTwistMotor.get_position() < TWIST_START_POSITION) {
        clawTwistMotor.move(127);
    } else {
        holdTwistPosition(TWIST_START_POSITION);
    }
}

// Position control for the claw twist. Assumes the motor and claw gear are about 1:1, and that
// negative encoder values move the claw toward scoring position.
// clawTwistMotor.tare_position() runs in runPreAutonomous(), so start with the claw at rest.
const double TWIST_REST_POSITION = 0.0;
const double TWIST_SCORING_POSITION = -900.0;  // Adjust to the actual scoring position
const double TWIST_MAX_POSITION = -1500.0;     // Tune for the physical upper limit
const int TWIST_POSITION_SPEED = 100;

void scoringReadyPosition() {
    clawTwistMotor.set_brake_mode(pros::MotorBrake::hold);
    clawTwistMotor.move_absolute(TWIST_SCORING_POSITION, TWIST_POSITION_SPEED);
}

void clawTwistMaxPosition() {
    clawTwistMotor.set_brake_mode(pros::MotorBrake::hold);
    clawTwistMotor.move_absolute(TWIST_MAX_POSITION, TWIST_POSITION_SPEED);
}

void restPosition() {
    clawTwistMotor.set_brake_mode(pros::MotorBrake::hold);
    clawTwistMotor.move_absolute(TWIST_REST_POSITION, TWIST_POSITION_SPEED);
}
