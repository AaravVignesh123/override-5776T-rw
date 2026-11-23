#pragma once

// Motor claw + twist (clawMotor and clawTwistMotor are in src/rw-config.cpp)
void storePin();
void releasePin();
void stopClawMotor();
void scoringReady();
void rest();
void scoringReady2();
void rest2();
void holdTwistPosition(double targetPosition);
void scoringReadyPosition();
void clawTwistMaxPosition();
void restPosition();
