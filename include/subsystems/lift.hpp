#pragma once
#include "main.h"

extern pros::Motor liftMotor1;
extern pros::Motor liftMotor2;

void liftUp();
void liftDown();
void liftToPosition(float position, int32_t velocity = 100);
void waitForLiftPosition(float position);
void tareLiftPosition();
