#pragma once
#include "main.h"

enum class ToggleAllianceColor {
    Red,
    Blue
};

extern pros::Motor rightToggleMotor;
extern pros::Motor leftToggleMotor;
extern ToggleAllianceColor toggleAllianceColor;
extern bool toggleColorSortEnabled;

std::int32_t initializeToggleColorSensor();
bool isNonAllianceColorDetected();
void toggleRollers();
void toggleOnce();