#include "rw-config.h"

void setClaw(bool open) { clawPiston.set_value(open); }
void openClaw() { setClaw(true); }
void closeClaw() { setClaw(false); }

int getClawDistanceMm() {
    return clawDistanceSensor.get();
}

bool autoGrab() {
    int currentDistance = clawDistanceSensor.get();
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
