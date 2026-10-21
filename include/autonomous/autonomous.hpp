#pragma once
#include "main.h"

enum liftUP {
    LIFT_UP,
    LIFT_DOWN
};

enum whichGoal {
    R_SIDE_ALLIANCE_GOAL,
    L_SIDE_ALLIANCE_GOAL,
    MIDDLE_GOAL
};

// Helper Functions
// Unload matchloader
void unloadMatchloader(Length x, bool useDelay = true, Angle angleOffset = 0_deg);
// Put descore wing in goal from long goal scoring position
void hookDescoreWing(Length x = 0_ft, Length y = 21_in);
void farSideMatchloader(Length x, Length y); // lemlib::Pose pose is the current pose of the robot, used to set the initial pose of the robot when starting the auton
void scoreFarSide(Length x, Length y, int pinStack);

// Override Helper Functions
void matchloadingRight(float objectHeight, enum whichGoal goal = R_SIDE_ALLIANCE_GOAL);

//Routes 
void eighteenPointAuto();
void LeftAuto(bool awp);
void RightAuto(bool awp);
void SoloAWP();
void skills();

// For turn pid
void turnTuning(); 

// For translation pid
void translationTuning();

// For finding MAX_SPEED_INCHES_PER_SECOND in speeds.hpp
void maxSpeedMeasuring();

// For testing experimental functions
void testingAuton();

void matchloading();