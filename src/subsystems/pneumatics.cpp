#include "main.h"
#include "subsystems/pneumatics.hpp"


int kPneumaticClawPort = 'A'; // HIGH - closed
pros::Distance claw_distance_sensor(10);

bool autoGrab() {
    int currentDistance = claw_distance_sensor.get();
    static bool cupAlreadyIn = false;

    if (0 < currentDistance && currentDistance < 30) {
        if (!cupAlreadyIn) {
            cupAlreadyIn = true;
            return true;
        }
    } else {
        cupAlreadIn = false;
    }
    return false;
}