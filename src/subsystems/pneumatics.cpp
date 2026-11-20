#include "main.h"
#include "subsystems/pneumatics.hpp"


int kPneumaticClawPort = 'A'; // LOW - closed
pros::Distance claw_distance_sensor(10);

int getClawDistanceMm() {
    return claw_distance_sensor.get();
}

bool autoGrab() {
    int currentDistance = claw_distance_sensor.get();
    static bool cupAlreadyIn = false;

    if (currentDistance > 0 && currentDistance < 20) {
        if (!cupAlreadyIn) {
            cupAlreadyIn = true;
            return true;
        }
    } else {
        cupAlreadyIn = false;
    }

    return false;
}