#pragma once
#include "main.h"

extern pros::Motor clawMotor;
extern pros::Motor clawTwistMotor;

void storePin();
void releasePin();
void scoringReady();
void rest();
void scoringReady2();
void rest2();
void holdTwistPosition(double targetPosition);
void scoringReadyPosition();
void restPosition();