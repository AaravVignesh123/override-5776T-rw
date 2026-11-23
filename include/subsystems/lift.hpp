#pragma once
#include <cstdint>

// Lift (liftMotor1 and liftMotor2 are in src/rw-config.cpp). Positions are in inches of lift travel
void liftUp();
void liftDown();
void liftStop();
void liftToPosition(float position, int32_t velocity = 100);
bool isLiftAtPosition(float position);
void waitForLiftPosition(float position, uint32_t timeoutMsec = 2000);
void tareLiftPosition();
