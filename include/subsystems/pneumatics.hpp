#pragma once

// Pneumatic claw on ADI port A (clawPiston in src/rw-config.cpp). LOW = closed, HIGH = open
void setClaw(bool open);
void openClaw();
void closeClaw();

// Auto-grab: returns true once each time an object comes into range of the claw distance sensor
bool autoGrab();
int getClawDistanceMm();
