#pragma once
#include <cstdint>

enum class ToggleAllianceColor { Red, Blue };

extern ToggleAllianceColor toggleAllianceColor;
extern bool toggleColorSortEnabled;

std::int32_t initializeToggleColorSensor();
bool isNonAllianceColorDetected();
void toggleRollers();
void stopToggle();
void toggleOnce();
