#include "main.h"
#include "subsystems/pneumatics.hpp"


int kPneumaticClawPort = 'A'; // HIGH - closed
pros::Distance claw_distance_sensor(10);

bool autoGrab() {
    int currentDistance = claw_distance_sensor.get();

    if (0 < currentDistance && currentDistance < 30) {
        pros::c::adi_digital_write(kPneumaticClawPort, HIGH);
        return true;
    }
    return false;
}