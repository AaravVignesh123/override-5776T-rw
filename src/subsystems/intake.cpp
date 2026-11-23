#include "rw-config.h"

// Intake is stubbed out on the current robot (see the port note in src/rw-config.cpp)
void intakePins() { intakeBottomLarge.move(127); }
void outtakePins() { intakeBottomLarge.move(-127); }
void stopIntake() { intakeBottomLarge.move(0); }
