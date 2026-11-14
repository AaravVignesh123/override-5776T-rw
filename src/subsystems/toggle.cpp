#include "subsystems/chassis.hpp"
#include "autonomous/odometry.hpp"
#include "lemlib/api.hpp"
#include "toggle.hpp"
#include "main.h"
#include "pros/optical.hpp"

//pros::Motor leftToggleMotor(-7);
pros::Motor rightToggleMotor(8);
pros::Optical toggleColorSensor(3);

// Change this to Red or Blue before the match.
ToggleAllianceColor toggleAllianceColor = ToggleAllianceColor::Red;
bool toggleColorSortEnabled = true;

namespace {
constexpr std::int32_t kMinimumColorProximity = 100;
constexpr double kMinimumColorSaturation = 0.25;
constexpr double kRedHueUpperBound = 30.0;
constexpr double kRedHueLowerBound = 330.0;
constexpr double kBlueHueLowerBound = 190.0;
constexpr double kBlueHueUpperBound = 260.0;
constexpr std::int32_t kToggleOnceVelocity = 130;
}

std::int32_t initializeToggleColorSensor() {
    return toggleColorSensor.set_led_pwm(100);
}

bool isNonAllianceColorDetected() {
    if (!toggleColorSortEnabled) {
        return false;
    }

    if (toggleColorSensor.get_proximity() < kMinimumColorProximity) {
        return false;
    }

    const double hue = toggleColorSensor.get_hue();
    const double saturation = toggleColorSensor.get_saturation();
    if (hue < 0 || hue >= 360 || saturation < kMinimumColorSaturation) {
        return false;
    }

    const bool isRed = hue <= kRedHueUpperBound || hue >= kRedHueLowerBound;
    const bool isBlue = hue >= kBlueHueLowerBound && hue <= kBlueHueUpperBound;
    const bool isAllianceColor = toggleAllianceColor == ToggleAllianceColor::Red ? isRed : isBlue;
    return !isAllianceColor;
}

void toggleRollers() {
    setMotorSpeed(rightToggleMotor, 100_power);
}
void toggleOnce(){
    rightToggleMotor.move_relative(1260, kToggleOnceVelocity);
}