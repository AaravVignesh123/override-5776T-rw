#include "main.h"
#include "autonomous/odometry.hpp"
#include "subsystems/chassis.hpp"
// #include "subsystems/intake.hpp"
#include "autonomous/autonomous.hpp"
#include "autonomous/odomDistance.hpp"
#include "subsystems/lift.hpp"
#include "subsystems/claw.hpp"
#include "subsystems/toggle.hpp"
#include "subsystems/pneumatics.hpp"
 
// Temporary stubs while the old autonomous logic is kept as reference.
void unloadMatchloader(Length x, bool useDelay, Angle angleOffset) {
    (void)x;
    (void)useDelay;
    (void)angleOffset;
}

void hookDescoreWing(Length x, Length y) {
    (void)x;
    (void)y;
}

#define ALIGN_TO_LONG_GOAL(time) do { (void)(time); } while (0)
#define ALIGN_TO_MIDDLE_GOAL(time) do { (void)(time); } while (0)


//Helper Functions --------------

// Lifting lift for a certain amount of time
// sets defualt delay to 100_ms for convienience, but can be changed if needed
// hmmmm should i make this function lift up and down sure
void liftingLift(enum liftUP liftDirection, Time timeWanted, Time delayTime = 100_ms) {
    Time startTime = getTime();
    while (getTime() - startTime < timeWanted) {
        if (liftDirection == LIFT_UP) {
            liftUp();
        } else {
            liftDown();
        }
        delay(delayTime);
    }
    //stops the lift afterward
    setMotorSpeed(liftMotor1, 0_x);
    setMotorSpeed(liftMotor2, 0_x);
}

/*
// Helper Functions ---------------------------------------
void unloadMatchloader(Length x, bool useDelay, Angle angleOffset) {
    turnToAngle(180_deg + angleOffset, 0.1_x);
    pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
    if (useDelay) {
        delay(150_ms);
    }
    storeBalls();
    moveToPoint(x, -15_in, true, 0.4_x, 0.7_x);
    setChassisSpeed(0.3_x, 0.3_x);
}

void hookDescoreWing(Length x, Length y) {
    // Initialize long goal position
    setChassisPose(x, y, 180_deg);

    // Hook in descore wing
    stopIntake();
    turnToAngle(115_deg, 1_x);
    swingToAngle(150_deg, lemlib::DriveSide::RIGHT, 0.7_x);
    // delay(150_ms);
    moveToPoint(x + 9_in, y + 23_in, false, 0.6_x, 1_x, 0.4_s);
    moveToPoint(x + 9_in, y + 23_in, false, 0_x, 0.6_x, 15_s, 0_in, true);  // No timeout to continue reversing until auton time ends
}

*/
void farSideMatchloader(Length x, Length y) {
    // Initialize matchloader position
    setChassisPose(x, y, 0_deg);
    // Get into matchloader
    relative(-3_in, 0.5_x);
    moveToPose(x+1.3_ft, y-1.37_ft, 180_deg, false, 0.4, 0.5_x, 1_x, 1_s);
    turnToAngle(180_deg, 0.4_x);
    moveToPose(x+1.52_ft, y-2.6_ft, 180_deg, true, 0.4, 0.5_x, 1_x, 1_s);
}
void scoreFarSide(Length x, Length y, int pinStack) {
    setChassisPose(x, y, 180_deg);
    // get to far side 
    moveToPose(x+1.3_ft, y-1.37_ft, 180_deg, false, 0.4, 0.5_x, 1_x, 1_s);
    turnToAngle(0_deg, 0.4_x);
    moveToPose(x-1.52_ft, y-2.6_ft-3_in, 0_deg, true, 0.4, 0.5_x, 1_x, 1_s, 0_in, true);
    liftToPosition(pinStack*4, 100);
    pros::c::adi_digital_write(kPneumaticClawPort, HIGH);

}
/*
// Macro to align into the long goal using driveStraightAtAngle(-0.5_x, 0.08_x, 180_deg, [time]);
#define ALIGN_TO_LONG_GOAL(time) driveStraightAtAngle(-0.6_x, 0.1_x, 180_deg, time);
#define ALIGN_TO_MIDDLE_GOAL(time) driveStraightAtAngle(-0.15_x, 0.02_x, -135_deg, time);

// Left Side Autonomous Route -----------------------------
void left(bool secureLongGoal) {
    // Initialize starting position
    setChassisPose(-3.5_in, 0.5_in, 0_deg);
    odomDistanceForward.setOriginPosition(-1_ft, -4_ft);
    odomDistanceRight.setOriginPosition(-1_ft, -4_ft);

    if (secureLongGoal) {
        // 4 balls in the long goal rush

        // Grab 3 balls by top middle goal
        storeBalls();
        moveToPose(-1_ft + 0.5_in, 2_ft - 0.5_in, -5_deg, true, 0.5, 0.6_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        
        // Score 4 balls into left long goal
        turnToAngle(-115_deg, 0.4_x, 1_x, 2_s);
        pros::c::adi_digital_write(kPneumaticScraperPort, LOW);
        setChassisSpeed(1_x, 1_x);
        delay(250_ms);
        moveToX(-3_ft + 1_in, -3_in, 9_in, 0.4_x, 1_x, 2_s);
        turnToAngle(-165_deg, 0.4_x);
        // odomDistanceRight.correctPosition();
        moveToPoint(-3_ft - 1_in, 14_in, false, 0.8_x);
        scoreBallsUp();
        ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG)

        // Hook descore arm into goal to secure control bonus
        hookDescoreWing(-3_ft, 20_in);

        // Hyperspeed 4 ball (lowest priority)
        /*
        // Grab 3 balls by top middle goal
        storeBalls();
        moveToPose(-1_ft + 0.5_in, 2_ft - 1_in, -15_deg, true, 0.5, 0.6_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        
        // Score 4 balls into left long goal
        turnToAngle(70_deg, 0.4_x);
        moveToX(-3_ft + 4_in, 6_in, 1_ft + 3_in, 0.5_x);
        swingToAngle(175_deg, lemlib::DriveSide::LEFT, 0.5_x);
        moveToPoint(-3_ft + 1_in, 11_in, false, 0.8_x);
        scoreBallsUp();
        ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG + 350_ms)
        pros::c::adi_digital_write(kPneumaticScraperPort, LOW);

        // Hook descore arm into goal to secure control bonus
        hookDescoreWing(-3_ft, 20_in);
        
    } else {
        // 7 balls in the long goal rush

        // Grab 3 balls by top middle goal
        storeBalls();
        moveToPose(-1_ft + 0.5_in, 2_ft - 3_in, -13_deg, true, 0.5, 0.6_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        
        // Unload 3 balls from left matchloader
        turnToAngle(-115_deg, 0.4_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, LOW);
        setChassisSpeed(1_x, 1_x);
        delay(250_ms);
        moveToX(-3_ft + 8_in, 4_in, 15_in, 0.6_x);
        // printPositionDebug();
        // turnToAngle(-175_deg, 0.4_x);
        // odomDistanceRight.correctPosition();
        moveToPoint(-3_ft + 2_in, -12_in, true, 0.4_x, 0.7_x);
        setChassisSpeed(0.3_x, 0.3_x);
        delay(MATCHLOAD_THREE_BLOCKS);
        odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);

        // Score 7 balls in left long goal
        moveToPoint(-3_ft + 2_in, 1_ft + 3_in, false, 0.8_x);
        scoreBallsUp();
        ALIGN_TO_LONG_GOAL(1_s)

        // Hook descore arm into goal to secure control bonus
        hookDescoreWing(-3_ft, 20_in);
    }
}

// Left Side AWP Assist Autonomous Route ------------------
void leftAwp(bool nineBall) {
    if (nineBall) {
        // 9 balls split 4/5 Long/Middle
        // Initialize starting position
        setChassisPose(-3.5_in, 0.5_in, 0_deg);
        odomDistanceForward.setOriginPosition(-1_ft, -4_ft);
        odomDistanceRight.setOriginPosition(-1_ft, -4_ft);

        // Grab 3 balls by top middle goal
        storeBalls();
        moveToPoint(-1_ft - 2_in, 2_ft, true, 0.5_x, 0.8_x, 1.5_s);

        // Grab 2 blocks from under left long goal
        moveToPose(-3_ft + 1_in, 3_ft + 4_in, -55_deg, true, 0.2, 0.4_x, 0.8_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        delay(100_ms);
        
        // Score 4 balls into left long goal
        setChassisSpeed(0.4_x, -1_x);
        delay(400_ms);
        swingToAngle(0_deg, lemlib::DriveSide::LEFT, 0.8_x);
        moveToPoint(-2_ft - 9_in, 2_ft + 1_in, false, 0.7_x);
        swingToAngle(160_deg, lemlib::DriveSide::LEFT, 1_x);
        swingToAngle(175_deg, lemlib::DriveSide::LEFT, 1_x);
        ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG)
        scoreBallsUp();
        ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG)

        // Correct position and get into left matchloader
        setChassisPose(-3_ft, 2_ft);
        moveToPose(-3_ft, -13_in, 180_deg, true, 0.4, 0.5_x, 1_x, 1.5_s, 0_in, true);
        scoreBallsDown();
        delay(200_ms);
        storeBalls();
        chassis.waitUntilDone();
    } else {
        // 7 balls split 4/3 Long/Middle
        // Run 4 ball left
        left(true);

        // Wait until as late as possible to leave the long goal
        while (getTime() - autonStartTime < 10_s) {
            delay(20_ms);
        }

        // Exit all motions if we're still trying to wing
        chassis.cancelMotion();

        // Correct position and get into left matchloader
        odomDistanceForward.ignoreTooLargeCorrections = true;
        odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
        odomDistanceForward.ignoreTooLargeCorrections = false;
        odomDistanceRight.correctPosition(OdomDistance::ForceToWall::LEFT_RIGHT);
        // pros::c::adi_digital_write(kPneumaticDescorePort, HIGH);
        moveToPoint(-2_ft - 1_in, 2_ft - 3_in, true, 0.5_x);
        turnToAngle(-165_deg, 0.4_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        storeBalls();
        moveToPose(-3_ft + 2_in, -14.5_in, 180_deg, true, 0.27, 0.5_x, 1_x, 1.5_s);
    }
    
    // Unload 3 blocks from left matchloader
    setChassisSpeed(0.4_x, 0.4_x);
    delay(MATCHLOAD_THREE_BLOCKS);
    odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    odomDistanceRight.correctPosition(OdomDistance::ForceToWall::LEFT_RIGHT);

    // Get into top middle goal
    moveToPoint(2_in, 3_ft + 2_in, false, 0_x, 0.75_x, 0.5_s);
    // moveToPose(1.5_in, 3_ft - 0.5_in, -135_deg, false, 0.5, 1_x, 1_x, 0.5_s);
    pros::c::adi_digital_write(kPneumaticMiddleDescorePort, HIGH);
    moveToPose(2_in, 3_ft + 2_in, -135_deg, false, 0.6, 0.6_x, 1_x, 0.8_s);
    moveToPoint(2_in, 3_ft + 2_in, false, 0.4_x, 1_x, 1_s, 7.5_in);

    // Score 3 blocks in top middle goal
    // turnToAngle(-135_deg, 0.4_x);
    // setChassisSpeed(-0.25_x, -0.25_x);
    scoreBallsDown();
    delay(CLEAR_HOOD_SECTION);
    scoreBallsMiddle();
    // setChassisSpeed(0_x, 0_x);
    delay(0.5_s);
    // ALIGN_TO_MIDDLE_GOAL(0.5_s)
    pros::c::adi_digital_write(kPneumaticScraperPort, LOW);
    pros::c::adi_digital_write(kPneumaticDescorePort, LOW);
    delay(10_s);
    // ALIGN_TO_MIDDLE_GOAL(5_s)

    // moveToX(-2_ft + 7.8_in, 9_in, 15_in, 0.1_x, 1_x, 2_s);
    // // moveToPoint(-2_ft + 7.5_in, 12_in);
    // turnToAngle(170_deg, 0.4_x);
    // moveToPoint(-2_ft + 1_in, 3_ft + 2_in, false);
    // turnToAngle(180_deg);
    // delay(200_ms);
}

// Right Side Autonomous Route ----------------------------
void right(bool secureLongGoal) {
    // Initialize starting position
    setChassisPose(3.5_in, 0.5_in, 0_deg);
    odomDistanceForward.setOriginPosition(1_ft, -4_ft);
    odomDistanceLeft.setOriginPosition(1_ft, -4_ft);

    if (secureLongGoal) {
        // 4 balls in the long goal rush

        // Grab 3 balls by bottom middle goal
        storeBalls();
        moveToPose(1_ft - 2.5_in, 2_ft - 2_in, 10_deg, true, 0.5, 0.6_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        
        // Score 4 balls into left long goal
        turnToAngle(105_deg, 0.4_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, LOW);
        setChassisSpeed(1_x, 1_x);
        delay(250_ms);
        moveToX(3_ft - 3_in, -3_in, 9_in, 0.4_x, 1_x, 1.5_s);
        turnToAngle(175_deg, 0.4_x);
        // odomDistanceLeft.correctPosition();
        moveToPoint(3_ft + 1_in, 15_in, false, 0.8_x);
        scoreBallsUp();
        ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG)

        // Hook descore arm into goal to secure control bonus
        hookDescoreWing(3_ft, 20_in);

        // Hyperspeed 4 ball (lowest priority)
        /*
        // Grab 3 balls by bottom middle goal
        storeBalls();
        moveToPose(1_ft - 0.5_in, 2_ft - 1_in, 15_deg, true, 0.5, 0.6_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        
        // Score 4 balls into right long goal
        turnToAngle(-70_deg, 0.4_x);
        moveToX(3_ft - 4_in, 6_in, 1_ft + 3_in, 0.5_x);
        swingToAngle(-175_deg, lemlib::DriveSide::LEFT, 0.5_x);
        moveToPoint(3_ft + 1_in, 11_in, false, 0.8_x);
        scoreBallsUp();
        ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG + 350_ms)
        pros::c::adi_digital_write(kPneumaticScraperPort, LOW);

        // Hook descore arm into goal to secure control bonus
        hookDescoreWing(3_ft, 20_in);
        
    } else {
        // 7 balls in the long goal rush

        // Grab 3 balls by bottom middle goal
        storeBalls();
        moveToPose(1_ft - 0.5_in, 2_ft - 1_in, 15_deg, true, 0.5, 0.6_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        delay(75_ms);
        
        // Unload 3 balls from right matchloader
        turnToAngle(125_deg, 0.4_x);
        setChassisSpeed(1_x, 1_x);
        delay(200_ms);
        moveToX(3_ft - 2_in, -5_in, 1_ft + 3_in, 0.3_x);
        turnToAngle(180_deg, 0.4_x);
        odomDistanceLeft.correctPosition();
        moveToPoint(3_ft + 2_in, -9_in, true, 0.4_x, 0.7_x);
        setChassisSpeed(0.3_x, 0.3_x);
        delay(MATCHLOAD_THREE_BLOCKS);
        odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);

        // Score 7 balls in right long goal
        moveToPoint(3_ft + 3_in, 1_ft + 3_in, false, 0.8_x);
        scoreBallsUp();
        ALIGN_TO_LONG_GOAL(1_s)
        pros::c::adi_digital_write(kPneumaticScraperPort, LOW);
        ALIGN_TO_LONG_GOAL(0.75_s)

        // Hook descore arm into goal to secure control bonus
        hookDescoreWing(3_ft, 20_in);
    }
}

// Right Side AWP Assist Autonomous Route -----------------
void rightAwp(bool nineBall) {
    if (nineBall) {
        // 9 balls split 4/5 Long/Middle
        // Initialize starting position
        setChassisPose(3.5_in, 0.5_in, 0_deg);
        odomDistanceForward.setOriginPosition(1_ft, -4_ft);
        odomDistanceLeft.setOriginPosition(1_ft, -4_ft);

        /*
        // Grab 3 balls by top middle goal
        storeBalls();
        pros::c::adi_digital_write(kPneumaticDescorePort, HIGH);
        moveToPoint(1_ft + 2_in, 2_ft + 2.5_in, true, 0.5_x, 0.8_x, 1.5_s);

        // Grab 2 blocks from under left long goal
        moveToPose(3_ft - 2_in, 3_ft + 7_in, 60_deg, true, 0.2, 0.4_x, 0.8_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        delay(100_ms);
        
        // Score 4 balls into left long goal
        setChassisSpeed(-1_x, 0.4_x);
        delay(400_ms);
        swingToAngle(0_deg, lemlib::DriveSide::RIGHT, 0.8_x);
        moveToPoint(2_ft + 9_in, 2_ft - 2_in, false, 0.7_x);
        swingToAngle(-160_deg, lemlib::DriveSide::RIGHT, 1_x);
        swingToAngle(-175_deg, lemlib::DriveSide::RIGHT, 1_x);
        scoreBallsUp();
        ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG)
    

        // Grab 3 balls by bottom middle goal
        storeBalls();
        moveToPose(1_ft - 2.5_in, 2_ft - 2_in, 10_deg, true, 0.5, 0.6_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        
        // Score 4 balls into left long goal
        turnToAngle(110_deg, 0.4_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, LOW);
        setChassisSpeed(1_x, 1_x);
        delay(250_ms);
        moveToX(3_ft - 0.5_in, -3_in, 9_in, 0.4_x, 1_x, 1.5_s);
        turnToAngle(175_deg, 0.4_x);
        // odomDistanceLeft.correctPosition();
        moveToPoint(3_ft + 1_in, 15_in, false, 0.8_x);
        ALIGN_TO_LONG_GOAL(250_ms)
        scoreBallsUp();
        ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG)

        // Correct position and get into right matchloader
        setChassisPose(3_ft, 2_ft);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        moveToPose(3_ft, -11.5_in, 180_deg, true, 0.4, 0.4_x, 0.6_x, 1.5_s, 0_in, true);
        scoreBallsDown();
        delay(200_ms);
        storeBalls();
        chassis.waitUntilDone();
    } else {
        // 7 balls split 4/3 Long/Middle
        // Run 4 ball right
        right(true);

        // Wait until as late as possible to leave the long goal
        while (getTime() < 11.5_s) {
            delay(20_ms);
        }

        // Exit all motions if we're still trying to wing
        chassis.cancelMotion();

        // Correct position and get into right matchloader
        odomDistanceForward.ignoreTooLargeCorrections = true;
        odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
        odomDistanceForward.ignoreTooLargeCorrections = false;
        pros::c::adi_digital_write(kPneumaticDescorePort, HIGH);
        moveToPoint(4_ft - 3_in, 2_ft - 3_in, true, 0.5_x);
        turnToAngle(-150_deg, 0.4_x);
        pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
        setMotorSpeed(intakeBottomLarge, 1_x);  // Only put balls at the bottom of the intake
        moveToPose(3_ft - 1_in, -14.5_in, 180_deg, true, 0.4, 0.5_x, 1_x, 1.5_s);
    }

    // Unload 3 blocks from right matchloader
    setChassisSpeed(0.2_x, 0.2_x);
    delay(MATCHLOAD_THREE_BLOCKS);
    odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    odomDistanceLeft.correctPosition(OdomDistance::ForceToWall::LEFT_RIGHT);

    // Score 3 blocks in bottom middle goal
    setMotorSpeed(intakeBottomSmall, -0.7_x);  // Stack up blocks at the bottom of the intake
    relative(-4_in, 0.4_x);
    turnToAngle(-60_deg, 0.3_x);
    pros::c::adi_digital_write(kPneumaticScraperPort, LOW);
    moveToPose(0_in, 3_ft - 3_in, -45_deg, true, 0.5, 0.7_x, 1_x, 2_s);
    pros::c::adi_digital_write(kPneumaticDescorePort, LOW);
    scoreBallsDown();
    if (nineBall) {
        setMotorSpeed(intakeBottomLarge, -0.7_x);
    }
    delay(5_s);
}

// Solo AWP Autonomous Route ------------------------------
void soloAwp() {
    // Counter solo sig awp split 4/4/6 Long/Middle/Long
    // Initialize starting position
    setChassisPose(-1_ft, 0_in, -90_deg);
    odomDistanceForward.setOriginPosition(1_ft, -4_ft);
    odomDistanceLeft.setOriginPosition(1_ft, -4_ft);
    odomDistanceRight.setOriginPosition(1_ft, -4_ft);
    odomDistanceBackwards.setOriginPosition(1_ft, -4_ft);
    storeBalls();
    relative(3_in, 0.4_x);
    delay(100_ms);

    // Unload 3 balls from right matchloader
    moveToPoint(3_ft - 2_in, 0_ft, false, 1_x, 1_x, 0.5_s);
    moveToX(3_ft - 5_in, -3_in, 3_in, 0.1_x, 1_x, 2_s);
    odomDistanceBackwards.correctPosition();
    // chassis.waitUntilDone();
    pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
    // turnToAngle(-175_deg, 0.4_x);
    storeBalls();
    turnToPoint(3_ft, -14_in, true, 0.3_x, 1_x, 1_s, -10_deg);
    odomDistanceLeft.correctPosition();
    moveToPoint(3_ft, -14_in, true, 0.3_x, 0.7_x);
    setChassisSpeed(0.2_x, 0.2_x);
    delay(MATCHLOAD_THREE_BLOCKS);
    odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    odomDistanceLeft.correctPosition(OdomDistance::ForceToWall::LEFT_RIGHT);
 
    // Score 4 balls in right long goal
    moveToPoint(3_ft, 14.5_in, false, 0.6_x);
    scoreBallsUp();
    ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG)
    pros::c::adi_digital_write(kPneumaticScraperPort, LOW);

    // Grab 3 balls by bottom middle goal
    setChassisPose(3_ft, 2_ft, chassisAngle());
    setChassisSpeed(1_x, -0.7_x);
    while (chassisAngle().abs() > 93_deg) {
        delay(10_ms);
    }
    storeBalls();
    moveToPoint(1_ft, 2_ft - 3_in, true, 0.8_x);

    // Grab 3 balls by top middle goal
    moveToPose(-3_ft - 2_in, 2_ft - 1.5_in, -90_deg, true, 0.4, 0.7_x, 1_x, 3_s);
    // pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
    delay(150_ms);
    odomDistanceForward.ignoreTooLargeCorrections = true;
    odomDistanceForward.correctPosition();

    // Score 4 balls in left long goal
    turnToAngle(-115_deg, 0.7_x);
    pros::c::adi_digital_write(kPneumaticDescorePort, HIGH);
    setChassisSpeed(1_x, 1_x);
    delay(250_ms);
    moveToX(-5_ft, -1_ft - 3_in, 1_ft + 9_in, 0.4_x, 1_x, 2_s);
    odomDistanceForward.correctPosition();
    odomDistanceForward.ignoreTooLargeCorrections = false;
    turnToPoint(-5_ft, 14_in, false, 0.4_x, 1_x, 1_s, -10_deg);
    moveToPoint(-5_ft, 14_in, false, 0.8_x);
    scoreBallsUp();
    ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG)
    setChassisPose(-5_ft, 20_in, chassisAngle());

    // Unload 3 balls from left matchloader
    moveToPoint(-5_ft, -14.5_in, true, 0.4_x, 0.8_x, 1_s, 0_in, true);
    // Clear top section while intaking on the bottom
    setMotorSpeed(intakeBottomLarge, 1_x);
    setMotorSpeed(intakeBottomSmall, -1_x);
    setMotorSpeed(intakeTop, -1_x);
    delay(CLEAR_HOOD_SECTION);
    setMotorSpeed(intakeBottomSmall, 0_x);
    setMotorSpeed(intakeTop, 0_x);
    chassis.waitUntilDone();
    setChassisSpeed(0.35_x, 0.35_x);
    delay(MATCHLOAD_THREE_BLOCKS);
    odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    odomDistanceRight.correctPosition(OdomDistance::ForceToWall::LEFT_RIGHT);

    // Get into top middle goal
    moveToPoint(-2_ft + 4.5_in, 3_ft, false, 0_x, 0.75_x, 0.5_s);
    // moveToPose(1.5_in, 3_ft - 0.5_in, -135_deg, false, 0.5, 1_x, 1_x, 0.5_s);
    pros::c::adi_digital_write(kPneumaticMiddleDescorePort, HIGH);
    moveToPose(-2_ft + 4.5_in, 3_ft, -135_deg, false, 0.5, 0.6_x, 1_x, 2_s);

    // Score 5 blocks in top middle goal
    pros::c::adi_digital_write(kPneumaticDescorePort, LOW);
    scoreBallsMiddle();
    delay(0.5_s);
    pros::c::adi_digital_write(kPneumaticScraperPort, LOW);
    delay(1_s);
}

void soloAwp2() {
    // Counter solo sig awp split 4/4/6 Long/Middle/Long
    // Initialize starting position
    setChassisPose(6_in, -3.5_in, 90_deg);
    // Something might be wrong with teh distance sensor origins
    odomDistanceForward.setOriginPosition(1_ft, -4_ft);
    odomDistanceLeft.setOriginPosition(1_ft, -4_ft);
    odomDistanceRight.setOriginPosition(1_ft, -4_ft);
    odomDistanceBackwards.setOriginPosition(1_ft, -4_ft);
    // storeBalls();
    // relative(3_in, 0.4_x);
    // delay(100_ms);
    
    // Unload 3 balls from right matchloader


    moveToPoint(2_ft + 3_in, -3.5_in, true, 0.4_x, 0.7_x, 1.5_s); // Was 2_ft + 4_in originally, but something may be wrong
    pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
    odomDistanceForward.correctPosition();
    // chassis.waitUntilDone();
    turnToAngle(170_deg, 0.3_x);
    storeBalls();

    relative(6_in, 0.4_x);
    // odomDistanceRight.correctPosition();
    setChassisSpeed(0.2_x, 0.2_x);
    delay(MATCHLOAD_THREE_BLOCKS);
    odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    odomDistanceLeft.correctPosition(OdomDistance::ForceToWall::LEFT_RIGHT);    

    // Score 4 balls in right long goal
    moveToPoint(3_ft, 2_ft - 9.5_in, false, 0.3_x, 0.6_x, 1.5_s);
    scoreBallsUp();
    ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG)
    pros::c::adi_digital_write(kPneumaticScraperPort, LOW);
    

    // Descore
    hookDescoreWing(2_ft + 9_in, 2_ft - 9.5_in);
    delay(500_ms);
    //pros::c::adi_digital_write(kPneumaticDescorePort, HIGH);
    odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    odomDistanceLeft.correctPosition();
    relative(1_ft, 0.4_x);


    // Pick Up 3 Balls near Low Goal
    //moveToPose(2_ft + 1_in, 1_ft + 4.5_in, -45_deg, true, 0.4, 0.7_x, 1_x, 2_s);
    moveToPoint(2_ft + 4_in, 1_ft + 4.5_in, true, 0.4_x, 0.7_x, 1.5_s);
    turnToAngle(-55_deg, 0.3_x);
    // moveToPose(2_ft + 1_in, 1_ft + 4.5_in, -58_deg, true, 0.4, 0.7_x, 1_x, 2_s);
    storeBalls();
    moveToPose(6_in, 3_ft - 7_in, -45_deg, true, 0.4, 0.2_x, 0.7_x, 5_s);
    relative(6_in, 0.2_x, 0.4_x);
    moveToPose(-9_in, 3_ft + 8_in, -45_deg, true, 0.4, 0.2_x, 0.7_x, 5_s); 
    delay(500_ms);
    //moveToPoint(6_in, 2_ft + 10_in, true, 0.4_x, 0.7_x, 1.5_s);

    // Score 3 balls into Low Goal
    // moveToPoint(4_in, 3_ft - 4_in, true, 0.4_x, 0.7_x, 1.5_s);
    scoreBallsDown();
    delay(1_s);

    // Grab 3 balls by Middle Goal
    moveToPoint(1_ft - 9_in, 2_ft + 8_in, false, 0.4_x, 0.7_x, 1.5_s);
    turnToAngle(-85_deg, 0.3_x);
    //moveToPose(1_ft, 2_ft, -90_deg, false, 0.4, 0.4_x, 0.7_x, 1_s);
    odomDistanceLeft.correctPosition();
    storeBalls();
    moveToPoint(-3_ft - 3_in, 2_ft, true, 0.25_x, 0.7_x, 1.5_s);
    pros::c::adi_digital_write(kPneumaticScraperPort, LOW);
    // odomDistanceLeft.ignoreTooLargeCorrections = true;
    // relative(3_ft + 9_in, 0.4_x);
    

    // Score 3 balls into Middle Goal
    turnToAngle(-120_deg, 0.4_x);
    moveToPose(-2_ft + 1.5_in, 3_ft + 4.5_in, -135_deg, false, 0.4, 0.4_x, 1_x, 3_s);
    //relative(1_ft + 4_in, -0.4_x);
    scoreBallsMiddle();
    delay(1_s);
    






    // moveToPose(2_ft + 4_in, -1_ft - 3.5_in, 180_deg, true, 0.4, 0.4_x, 0.7_x, 1.5_s);
    // moveToPoint(2_ft + 4_in, -1_ft + -3.5_in, true, 0.4_x, 0.7_x, 1.5_s);

    /* moveToPoint(3_ft - 2_in, 0_ft, false, 1_x, 1_x, 0.5_s);
    // moveToX(3_ft - 5_in, -3_in, 3_in, 0.1_x, 1_x, 2_s);
    // odomDistanceBackwards.correctPosition();
    // chassis.waitUntilDone();
    // pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
    // turnToAngle(-175_deg, 0.4_x);
    // storeBalls();
    turnToPoint(3_ft, -14_in, true, 0.3_x, 1_x, 1_s, -10_deg); 
    odomDistanceLeft.correctPosition();
    moveToPoint(3_ft, -14_in, true, 0.3_x, 0.7_x);
    setChassisSpeed(0.2_x, 0.2_x);
    delay(MATCHLOAD_THREE_BLOCKS);
    odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    odomDistanceLeft.correctPosition(OdomDistance::ForceToWall::LEFT_RIGHT);
 
    // Score 4 balls in right long goal
    moveToPoint(3_ft, 14.5_in, false, 0.6_x);
    scoreBallsUp();
    ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG)
    pros::c::adi_digital_write(kPneumaticScraperPort, LOW);

    // Grab 3 balls by bottom middle goal
    setChassisPose(3_ft, 2_ft, chassisAngle());
    setChassisSpeed(1_x, -0.7_x);
    while (chassisAngle().abs() > 93_deg) {
        delay(10_ms);
    }
    storeBalls();
    moveToPoint(1_ft, 2_ft - 3_in, true, 0.8_x);

    // Grab 3 balls by top middle goal
    moveToPose(-3_ft - 2_in, 2_ft - 1.5_in, -90_deg, true, 0.4, 0.7_x, 1_x, 3_s);
    // pros::c::adi_digital_write(kPneumaticScraperPort, HIGH);
    delay(150_ms);
    odomDistanceForward.ignoreTooLargeCorrections = true;
    odomDistanceForward.correctPosition();

    // Score 4 balls in left long goal
    turnToAngle(-115_deg, 0.7_x);
    pros::c::adi_digital_write(kPneumaticDescorePort, HIGH);
    setChassisSpeed(1_x, 1_x);
    delay(250_ms);
    moveToX(-5_ft, -1_ft - 3_in, 1_ft + 9_in, 0.4_x, 1_x, 2_s);
    odomDistanceForward.correctPosition();
    odomDistanceForward.ignoreTooLargeCorrections = false;
    turnToPoint(-5_ft, 14_in, false, 0.4_x, 1_x, 1_s, -10_deg);
    moveToPoint(-5_ft, 14_in, false, 0.8_x);
    scoreBallsUp();
    ALIGN_TO_LONG_GOAL(FOUR_BALLS_INTO_LONG)
    setChassisPose(-5_ft, 20_in, chassisAngle());

    // Unload 3 balls from left matchloader
    moveToPoint(-5_ft, -14.5_in, true, 0.4_x, 0.8_x, 1_s, 0_in, true);
    // Clear top section while intaking on the bottom
    setMotorSpeed(intakeBottomLarge, 1_x);
    setMotorSpeed(intakeBottomSmall, -1_x);
    setMotorSpeed(intakeTop, -1_x);
    delay(CLEAR_HOOD_SECTION);
    setMotorSpeed(intakeBottomSmall, 0_x);
    setMotorSpeed(intakeTop, 0_x);
    chassis.waitUntilDone();
    setChassisSpeed(0.35_x, 0.35_x);
    delay(MATCHLOAD_THREE_BLOCKS);
    odomDistanceForward.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    odomDistanceRight.correctPosition(OdomDistance::ForceToWall::LEFT_RIGHT);

    // Get into top middle goal
    moveToPoint(-2_ft + 4.5_in, 3_ft, false, 0_x, 0.75_x, 0.5_s);
    // moveToPose(1.5_in, 3_ft - 0.5_in, -135_deg, false, 0.5, 1_x, 1_x, 0.5_s);
    pros::c::adi_digital_write(kPneumaticMiddleDescorePort, HIGH);
    moveToPose(-2_ft + 4.5_in, 3_ft, -135_deg, false, 0.5, 0.6_x, 1_x, 2_s);

    // Score 5 blocks in top middle goal
    pros::c::adi_digital_write(kPneumaticDescorePort, LOW);
    scoreBallsMiddle();
    delay(0.5_s);
    pros::c::adi_digital_write(kPneumaticScraperPort, LOW);
    delay(1_s);
    
}

//18 point auto from the video
void eighteenPointAuto() {
    //starting against left wall, front touching wall
    setChassisPose(0_ft, 0_ft, -90_deg);

    //toggle x2
    relative(-10_in, 0.35_x);
    relative(10_in, 0.35_x);

    relative(-10_in, 0.35_x);
    relative(10_in, 0.35_x);

    //2 ft away from the wall
    moveToPoint(0_ft, -2_ft, true, 0.4_x, 0.7_x, 0.75_s);
    //turn so that the back of the bot is facing the goal
    turnToAngle(0_deg, 0.5_x);

    moveToPoint(-2_ft, -2_ft, true, 0.4_x, 0.7_x, 0.75_s);
    //lift up
    //lift down, assuming we make a function for this later
    moveToPoint(0_ft, -2_ft, false, 0.4_x, 0.7_x, 0.75_s);
    turnToAngle(-90_deg, 0.5_x);

    //drive backwards 2 ft to pick up the cup and cone
    moveToPoint(0_ft, -4_ft, true, 0.4_x, 0.7_x, 0.75_s);
    //clamp function
}
*/

void LeftAuto(){
    // Initiialize starting pos
    setChassisPose(0.13_ft, -1.48_ft+1_in, -50_deg); // spin bot find center of rotation find distance from odom and set that as chassis pose
    // needa change these positions & offsets  (including reading)
    odomDistanceForward.setOriginPosition(4_in, -10_in);
    odomDistanceLeft.setOriginPosition(1_ft, -4_ft);
    odomDistanceRight.setOriginPosition(0_in, -1_ft-7_in);
    odomDistanceBackwards.setOriginPosition(0_in, -18_in);
    
    // toggle
    toggleOnce();
    delay(600_ms);
    
    //moveToPoint(0_ft, -3_in, false, 0.4_x, 0.7_x, 1.5_s); 
    //releasePin();
    liftToPosition(2,100);
    //move to alliance goal to score preload
    moveToPose(-1.5_ft - 6.5_in, -9_in, -90_deg, true, 0.6, 0.6_x, 0.8_x, 2_s, 0_in, false);

    //turnToAngle(90_deg, 0.5_x);
    //score preload
    delay(250_ms); 
    liftToPosition(0,100);
    delay(350_ms);
    pros::c::adi_digital_write(kPneumaticClawPort, HIGH);
    odomDistanceLeft.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);

    //delay(650_ms);
    //go to standing pin #1 and grab pin
    relative(-5_in, 0.4_x);
    turnToAngle(-35_deg, 0.5_x);
    moveToPoint(-2_ft+1_in, 1_ft+2_in, true, 0.5_x, 0.55_x, 1.5_s); // move to standing pin
    pros::c::adi_digital_write(kPneumaticClawPort, LOW);
    //moveToPose(-1.3_ft, 1_ft, -45_deg, true, 0.5, 0.6_x, 1_x, 2_s);

    delay(200_ms); //needed delay

    // go to alliance goal to score pin
    liftToPosition(8,100);
    turnToAngle(175_deg, 0.35_x);
    delay(250_ms);
    moveToPoint(-2_ft, 5_in, true, 0.6_x, 0.6_x, 2_s); // move to blue goal
    delay(200_ms); //needed delays
    liftToPosition(5,100);
    delay(200_ms);
    odomDistanceBackwards.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    pros::c::adi_digital_write(kPneumaticClawPort, HIGH);

    // move to second standing pin
    relative(-5_in, 0.4_x);
    turnToAngle(-130_deg, 0.5_x);
    delay(250_ms);
    liftToPosition(0,100);
    moveToPoint(-3_ft-4_in, 0_in, true, 0.4_x, 0.45_x, 1.5_s); // move to standing pin

    //grab second standing pin
    delay(150_ms);
    pros::c::adi_digital_write(kPneumaticClawPort, LOW);
    delay(150_ms);

    //lift up and position towards goal
    liftToPosition(11,100);
    //turnToAngle(90_deg, 0.35_x);
    turnToAngle(90_deg, 0.35_x);
    delay(310_ms);
    
    //move to alliance/blue goal
    moveToPoint(-2_ft-6_in, -2_in, true, 0.4_x, 0.5_x, 1.5_s); // move to blue goal
    
    //score second standing pin - end of four pin auto (next steps is getting wall pin)
    liftToPosition(10,100);
    odomDistanceRight.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    delay(250_ms);
    pros::c::adi_digital_write(kPneumaticClawPort, HIGH);

    //move into position for wall pin
    liftToPosition(0,100);
    relative(-6_in, 0.4_x);
    turnToAngle(143_deg, 0.5_x);
    turnToAngle(135_deg, 0.5_x);
    // moveToPoint(-2_ft-4.5_in, -1_ft+2.5_in, true, 0.3_x, 0.4_x, 1.5_s); // move to standing pin
    moveToPoint(-2_ft-6_in+0.34_ft, -2_in - 0.75_ft,true, 0.3_x, 0.4_x, 1.5_s); // move to standing pin
    delay(350_ms);

    // do the method of spinning one side of drive to get in prime posiition to pick pin
    swingToAngle(180_deg, lemlib::DriveSide::RIGHT, 0.3_x);
    pros::c::adi_digital_write(kPneumaticClawPort, LOW);
    relative(1.5_in, 0.4_x);
    delay(250_ms);

    relative(-1.5_in, 0.4_x);
    swingToAngle(135_deg, lemlib::DriveSide::RIGHT, 0.3_x);
    moveToPose(-3_ft-4_in, 0_in, 135_deg, false, 0.4, 0.4_x, 0.5_x, 1.5_s);
    liftToPosition(8,100);
    turnToAngle(0_deg,0.5_x);
    moveToPose(-3_ft-4_in,1_ft, 0_deg, true, 0.4, 0.4_x, 0.5_x, 1.5_s);
    pros::c::adi_digital_write(kPneumaticClawPort, HIGH);


    //move back and turn towards neutral goal
    // relative(-1_in, 0.4_x);
    /*
    turnToAngle(90_deg, 0.5_x);

    moveToPoint(-1_ft, -1_ft, true, 0.5_x, 0.8_x, 1.5_s); // move to neutral goal - p1
    moveToPose(0_ft, 0_ft, 90_deg, true, 0.5, 0.8_x, 1_x, 2_s); // move to neutral goal - p2 - function 1 - intermediate
    liftToPosition(4,100);
    moveToPose(6_in, 0_ft, 90_deg, true, 0.5, 0.6_x, 0.7_x, 2_s); // move to neutral goal
    liftToPosition(2,100);
    pros::c::adi_digital_write(kPneumaticClawPort, HIGH); //score wall pin

    //0.75 0.34 43.11
    */
}
void RightAuto(){
    // Initiialize starting pos
    setChassisPose(0.13_ft, -1.48_ft, 45_deg); // spin bot find center of rotation find distance from odom and set that as chassis pose
    // needa change these positions & offsets  (including reading)
    odomDistanceForward.setOriginPosition(4_in, -10_in);
    odomDistanceLeft.setOriginPosition(1_ft, -4_ft);
    odomDistanceRight.setOriginPosition(0_in, -1_ft-7_in);
    odomDistanceBackwards.setOriginPosition(0_in, -18_in);
    
    // toggle
    toggleOnce();
    delay(800_ms);
    
    //moveToPoint(0_ft, -3_in, false, 0.4_x, 0.7_x, 1.5_s); 
    //releasePin();
    liftToPosition(2,100);
    //move to alliance goal to score preload
    //moveToPose(-1.5_ft - 6.5_in, -9_in, 90_deg, true, 0.6, 0.7_x, 1_x, 2_s, 0_in, false);

    

    // //turnToAngle(90_deg, 0.5_x);
    // //score preload
    // delay(250_ms); 
    // liftToPosition(0,100);
    // delay(350_ms);
    // pros::c::adi_digital_write(kPneumaticClawPort, HIGH);
    // odomDistanceLeft.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);


    // //delay(650_ms);
    // //go to standing pin #1 and grab pin
    // relative(-5_in, 0.4_x);
    // turnToAngle(-35_deg, 0.5_x);
    // moveToPoint(-2_ft+1_in, 1_ft+2_in, true, 0.5_x, 0.55_x, 1.5_s); // move to standing pin
    // pros::c::adi_digital_write(kPneumaticClawPort, LOW);
    // //moveToPose(-1.3_ft, 1_ft, -45_deg, true, 0.5, 0.6_x, 1_x, 2_s);

    // delay(200_ms); //needed delay

    // // go to alliance goal to score pin
    
    // liftToPosition(8,100);
    // turnToAngle(175_deg, 0.35_x);
    // delay(250_ms);
    // moveToPoint(-2_ft, 4_in, true, 0.6_x, 0.6_x, 2_s); // move to blue goal
    // delay(200_ms); //needed delays
    // liftToPosition(5,100);
    // delay(200_ms);
    // odomDistanceBackwards.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    // pros::c::adi_digital_write(kPneumaticClawPort, HIGH);


    // // move to second standing pin
    // relative(-5_in, 0.4_x);
    // turnToAngle(-130_deg, 0.5_x);
    // delay(250_ms);
    // liftToPosition(0,100);
    // moveToPoint(-3_ft-3_in, -1_in, true, 0.4_x, 0.45_x, 1.5_s); // move to standing pin

    // //grab second standing pin
    // delay(150_ms);
    // pros::c::adi_digital_write(kPneumaticClawPort, LOW);
    // delay(150_ms);

    // //lift up and position towards goal
    // liftToPosition(11,100);
    // //turnToAngle(90_deg, 0.35_x);
    // turnToAngle(90_deg, 0.35_x);
    // delay(310_ms);
    
    // //move to alliance/blue goal
    // moveToPoint(-2_ft-6_in, -2_in, true, 0.4_x, 0.5_x, 1.5_s); // move to blue goal
    
    // //score second standing pin - end of four pin auto (next steps is getting wall pin)
    // liftToPosition(10,100);
    // odomDistanceRight.correctPosition(OdomDistance::ForceToWall::FRONT_BACK);
    // delay(250_ms);
    // pros::c::adi_digital_write(kPneumaticClawPort, HIGH);


    // //move into position for wall pin
    // liftToPosition(0,100);
    // relative(-6_in, 0.4_x);
    // turnToAngle(145_deg, 0.5_x);
    // moveToPoint(-2_ft-1_in, -1_ft-4_in, true, 0.4_x, 0.4_x, 1.5_s); // move to standing pin


    // // do the method of spinning one side of drive to get in prime posiition to pick pin
    // swingToAngle(180_deg, lemlib::DriveSide::RIGHT, 0.8_x);
    // pros::c::adi_digital_write(kPneumaticClawPort, LOW);

    // //move back and turn towards neutral goal
    // relative(-1_in, 0.4_x);
    // turnToAngle(90_deg, 0.5_x);

    // moveToPose(0_ft, 0_ft, 90_deg, true, 0.5, 0.8_x, 1_x, 2_s); // move to neutral goal - function 1 - intermediate
    // liftToPosition(4,100);
    // moveToPose(6_in, 0_ft, 90_deg, true, 0.5, 0.6_x, 0.7_x, 2_s); // move to neutral goal
    // liftToPosition(2,100);
    // pros::c::adi_digital_write(kPneumaticClawPort, HIGH); //score wall pin
    
    


    // // Initiialize starting pos
    // setChassisPose(0_ft, -1.1_ft, 180_deg); // spin bot find center of rotation find distance from odom and set that as chassis pose
    // // needa set up distance sensors when ready

    
    // // toggle
    // relative(-0.5_ft, 1.5_x);
    // moveToPoint(0_ft, -1.1_ft - 7_in, true, 1.5_x, 1.6_x, 1_s); // move to starting point plus a bit of y
    // relative(-0.5_ft, 1.5_x);
    // moveToPoint(0_ft, -1.1_ft - 7_in, true, 1.5_x, 1.6_x, 1_s); // move to starting point plus a bit of y

    
    // moveToPoint(0_ft, -3_in, false, 0.4_x, 0.7_x, 1.5_s); 
    // //releasePin();
    // liftingLift(LIFT_UP, 150_ms);
    // /*
    // Time startTime = getTime();
    // while (getTime() - startTime < 0.5_s) {
    //     liftUp();
    //     //releasePin();
    //     //liftUp();
    //     delay(100_ms);
    // }
    //     */

    // setMotorSpeed(liftMotor1, 0_x);
    // setMotorSpeed(liftMotor2, 0_x);
    // scoringReadyPosition();
    // turnToAngle(-90_deg, 0.5_x);
    // moveToPoint(1.6_ft - 1_in, 2_in, false, 0.4_x, 0.5_x, 1.5_s); // move to black goal
    // // moveToPose(-1.6_ft + 1_in, -3_in, 90_deg, false, 0.4, 0.6_x, 1_x, 2_s); // move to black goal
    // //delay(750_s);
    // Time startTime2 = getTime();
    // while (getTime() - startTime2 < 1.2_s) {
    //     liftDown();
    //     //releasePin();
    //     //liftUp();
    //     delay(100_ms);
    // }
    
    // setMotorSpeed(liftMotor1, 0_x);
    // setMotorSpeed(liftMotor2, 0_x);
    // delay(100_ms);
    // Time startTime9 = getTime();
    // while (getTime() - startTime9 < 0.45_s) {
    //     storePin();
    //     //releasePin();
    //     //liftUp();
    //     delay(50_ms);
    // }
    
    
    
    // relative(6_in, 0.4_x);
    // turnToAngle(-145_deg, 0.5_x);
    // //moveToPose(-2_ft, 2_ft, 80_deg, false, 0.5, 0.6_x, 1_x, 2_s);  move to standing pin
    
    
    
    
    // clawTwistMaxPosition();
    // moveToPose(2_ft-2_in, 1_ft + 9_in, -145_deg, false, 0.5, 0.4_x, 0.6_x, 2_s); // move to standing pin
    //  //lift claw up
    // delay(100_ms);
    // Time startTime5 = getTime();
    // //claw down and store pin and claw down (check if it can go to mid position)
    // while(getTime() - startTime5 < 0.5_s) {
    //     releasePin();
    //     //liftUp();
    //     //scoringReadyPosition();
    //     delay(250_ms);
    // }
    // setMotorSpeed(liftMotor1, 0_x);
    // setMotorSpeed(liftMotor2, 0_x);

    // //rest();
    // //go to mid claw position
    // Time startTime29 = getTime();
    // while(getTime() - startTime29 < 0.3_s) {
    //     rest();
    //     delay(100_ms);
    // }
    // scoringReadyPosition();
    // delay(100_ms);
    // //lift claw up to score
    // Time startTime6 = getTime();
    // while(getTime() - startTime6 < 0.7_s) {
    //     liftUp();
    //     //releasePin();
    //     //liftUp();
    //     delay(100_ms);
    // }
    //     liftUp();
    //     //releasePin();
    //     //liftUp();
    //     delay(250_ms);
    // setMotorSpeed(liftMotor1, 0_x);
    // setMotorSpeed(liftMotor2, 0_x);

    // //move to goal
    // turnToAngle(0_deg, 0.3_x); //turn to goal
    // Time startTime91 = getTime();
    // while(getTime() - startTime91 < 0.6_s) {
    //     liftUp();

    //     delay(100_ms);
    // }
    // setMotorSpeed(liftMotor1, 0_x);
    // setMotorSpeed(liftMotor2, 0_x);
    // moveToPoint(2_ft+4_in, 0_ft + 8_in, false, 0.3_x, 0.4_x, 1.5_s); // move to blue goal
    // //moveToPose(-2_ft, 0_ft + 4_in, 0_deg, false, 0.5, 0.6_x, 1_x, 2_s); // move to blue goal
    // delay(200_ms);

    // storePin();
    
    // // everything else check later
    // // ready to go to left standing pin
    // delay(400_ms);
    // clawTwistMaxPosition();
    // storePin();
    
    // relative(6_in, 0.4_x);

    // //Go to left standing pin
    // turnToAngle(-45_deg, 0.5_x);
    // //neeed to look for lift up and release pin functions
    // moveToPoint(3_ft+5_in, 7_in, false, 0.5_x, 0.6_x, 1.5_s); // move to right standing pin
    // //moveToPose(-3.5_ft, -0.5_ft, -90_deg, false, 0.5, 0.6_x, 1_x, 2_s);
    // delay(200_ms); // move to right standing pin
    // Time startTime7 = getTime();
    // while (getTime() - startTime7 < 1.5_s) {
    //     liftDown();
    //     releasePin();
    //     //liftUp();
    //     delay(250_ms);
    // }
    // Time startTime31 = getTime();
    // while(getTime() - startTime31 < 0.3_s) {
    //     rest();
    //     delay(100_ms);
    // }
    // //rest();
    // scoringReadyPosition();
    // delay(200_ms);

    // turnToAngle(-90_deg, 0.5_x);
    // Time startTime8 = getTime();
    // while (getTime() - startTime8 < 1.5_s) {
    //     liftUp();
    //     //releasePin();
    //     //liftUp();
    //     delay(250_ms);
    // }
    // turnToAngle(0_deg, 0.5_x);
    // intakePins();
    // moveToPose(1.8_ft, 2_ft, 30_deg, true, 0.5, 0.6_x, 1_x, 2_s); // move to flat pins
    // setMotorSpeed(liftMotor1, 0_x);
    // setMotorSpeed(liftMotor2, 0_x);
    // storePin();
    // moveToPose(1_ft + 2_in, 0_ft + 5_in, 90_deg, false, 0.3, 0.4_x, 0.5_x, 2_s); // move to blue goal
    // storePin();


}

void SoloAWP(){
    // Initiialize starting pos
    setChassisPose(0_ft, -1.1_ft, 180_deg); // spin bot find center of rotation find distance from odom and set that as chassis pose
    // needa set up distance sensors when ready

    
    // toggle
    relative(-0.5_ft, 1.8_x);
    moveToPoint(0_ft, -1.1_ft - 7_in, true, 1.5_x, 1.6_x, 1_s); // move to startining point plus a bit of y
    relative(-0.5_ft, 1.8_x);
    moveToPoint(0_ft, -1.1_ft - 9_in, true, 1.5_x, 1.6_x, 1_s); // move to starting point plus a bit of y


    moveToPoint(0_ft, 1_in, false, 0.4_x, 0.7_x, 1.5_s); //right alliance goal
    //releasePin();
    Time startTime = getTime();
    while (getTime() - startTime < 0.3_s) {
        liftUp();
        //releasePin();
        //liftUp();
        delay(100_ms);
    }

    setMotorSpeed(liftMotor1, 0_x);
    setMotorSpeed(liftMotor2, 0_x);
    scoringReadyPosition();
    turnToAngle(-90_deg, 0.5_x);
    moveToPose(1.6_ft + 3_in, 0_ft + 3_in, -90_deg, false, 0.5, 0.6_x, 1_x, 1.5_s); // move to blue goal
    delay(650_ms);
    Time startTime2 = getTime();
    while (getTime() - startTime2 < 0.4_s) {
        liftDown();
        //releasePin();
        //liftUp();
        delay(100_ms);
    }
    setMotorSpeed(liftMotor1, 0_x); 
    setMotorSpeed(liftMotor2, 0_x);
    storePin();
    moveToPoint(1.6_ft + 3_in, 0_ft + 8_in, false, 0.5_x, 0.6_x, 0.5_s);
    turnToAngle(-45_deg, 0.5_x);
    moveToPose(3.8_ft, 0_ft + 3_in, -90_deg, false, 0.5, 0.4_x, 0.7_x, 2_s); // fix angle
    delay(650_ms);
    Time startTime0 = getTime();
    while (getTime() - startTime0 < 0.4_s) {
        liftUp();
        // I need to intake the pin
        // liftUp();
        delay(100_ms);
    }
    relative(-5_in, 2_x); // added code
    turnToAngle(-135_deg, 0.5_x);
    moveToPose(2_ft, -1_ft, -90_deg, false, 0.4, 0.7_x, 1.0_x, 0.5_s); // between alliance goal and the border
    clawTwistMaxPosition();
    moveToPose(0_ft, 1_ft + 10_in, -90_deg, false, 0.4, 0.7_x, 1.0_x, 1.5_s); //at the cup and claw
    storePin();
    restPosition();
    scoringReadyPosition(); // uhh I'm not too sure if this is how you code it but it's supposed to be getting the cup with the pin at this point
    delay(650_ms);
    Time startTime66 = getTime();
    while (getTime() - startTime66 < 0.4_s) {
        liftUp();
        //liftUp();
        delay(100_ms);
    }
    turnToAngle(90_deg, 0.5_x); // yo why the angles different
    moveToPose(2_ft + 3_in, 0_ft + 3_in, -90_deg, false, 0.5, 0.6_x, 1_x, 1.5_s); // move to alliance color goal
    delay(650_ms);
    Time startTime67 = getTime();
    while (getTime() - startTime67 < 0.4_s) {
        liftDown();
        //releasePin();
        //liftUp();
        delay(100_ms);
    }
    setMotorSpeed(liftMotor1, 0_x);
    setMotorSpeed(liftMotor2, 0_x);
    storePin(); // pin and cup dropped in the alliance goal, hopefully
    turnToAngle(0_deg, 0.5_x);
    moveToPose(2_ft, 2_ft, 90_deg, false, 0.4, 0.7_x, 1.0_x, 1.5_s); // at the pin and cup
    storePin();
    restPosition();
    scoringReadyPosition(); // once again not sure if this is how you code it but it's supposed to be getting the cup with the pin at this point
    delay(650_ms);
    Time startTime88 = getTime();
    while (getTime() - startTime88 < 0.4_s) {
        liftUp();
        delay(100_ms);
    }
    turnToAngle(135_deg, 0.5_x); // facing the black goal
    moveToPose(-2_ft, 0_ft + 3_in, 90_deg, false, 0.4, 0.7_x, 1.0_x, 1.5_s); // at the black goal
    delay(650_ms);
    Time startTime68 = getTime();
    while (getTime() - startTime68 < 0.4_s) {
        liftDown();
        //releasePin();
        //liftUp();
        delay(100_ms);
    }
    setMotorSpeed(liftMotor1, 0_x);
    setMotorSpeed(liftMotor2, 0_x);
    storePin(); // should've released the pin...
}


// Skills Route -------------------------------------------
void skills() {

}

// Misc Autons --------------------------------------------
void turnTuning() {
    setChassisPose(0_ft, 0_ft, 0_deg);
    double startAngle = imu.get_heading();
    pros::lcd::print(3, "Start: %f", startAngle);

    turnToAngle(90_deg);
    
    double endAngle = imu.get_heading();
    pros::lcd::print(4, "End: %f", endAngle);
    pros::lcd::print(5, "Difference: %f", endAngle - startAngle - 90);
    delay(2_s);

    // turnToAngle(0_deg);
}

void translationTuning() {
    setChassisPose(0_ft, 0_ft, 0_deg);
    double startY = chassis.getPose().y / 12;
    pros::lcd::print(3, "Start: %f", startY);

    relative(2_ft);
    
    double endY = chassis.getPose().y / 12;
    pros::lcd::print(4, "End: %f", endY);
    pros::lcd::print(5, "Difference: %f", endY - startY - 2);
    delay(2_s);

    // relative(-2_ft, 0_x, 0.3_x, TRANSLATE_TIMEOUT_LONG);
    //moveToPoint(0_ft, 0_ft, false, 0_x, 0.3_x, 3_s);
}

void maxSpeedMeasuring() {
    double maxRecordedSpeed = 0;

    // pros::Task speedTask([&]() {
    //     while (true) {
    //         maxRecordedSpeed = std::max(maxRecordedSpeed, std::abs((float)trackingWheelFBSensor.get_velocity()) / 100.0 / 360.0 * M_PI * 2 * 2.75);
    //         delay(20_ms);
    //     }
  	// });

    relative(3_ft, 0_x, 1_x, TRANSLATE_TIMEOUT_LONG);
    pros::lcd::print(3, "Max Speed (in/sec): %f", maxRecordedSpeed);
}

void testingAuton() {
    // setChassisPose(0_ft, 0_ft, 0_deg);
    // turnToAngle(90_deg);
    // printPositionDebug();

    // odomDistanceForward.setOriginPosition(-4_ft, 3_ft);
    // odomDistanceLeft.setOriginPosition(-4_ft, 3_ft);
    // odomDistanceRight.setOriginPosition(-4_ft, -3_ft);
    odomDistanceBackwards.setOriginPosition(-4_ft, -4_ft);
    setChassisPose(0_ft, 0_ft, 0_deg);
    // turnToAngle(180_deg);
    delay(500_ms);
    // odomDistanceForward.correctPosition();
    // odomDistanceLeft.correctPosition();
    // odomDistanceRight.correctPosition();
    odomDistanceBackwards.correctPosition();

    // setChassisPose(-2_ft + 5_in, 3_ft + 7_in, -135_deg);
    // moveToX(-5_ft, -3_in, 6_in, 0_x, 1_x, 2_s);
    // printPositionDebug();

    // relative(3_in, 0.3_x);

    // setIntakeMode(SPLIT);
    // setChassisPose(0_ft, 0_ft, 0_deg);
    // score7IntoMiddle();

    // setChassisPose(0_ft, 0_ft, 180_deg);
    // moveToPose(-1.5_in, -1_ft, 180_deg, true, 0.6, 0.4_x, 1_x, 2_s);
    // moveToPose(-1.5_in, 0_ft, 180_deg, false, 0.6, 0.5_x);

    // driveStraightAtAngle(-0.5_x, 0.08_x, 180_deg, 2_s);

    // setChassisPose(0_ft, 0_ft, 180_deg);
    // moveToPose(1.5_in, -1_ft, 180_deg, true, 0.6, 0.4_x, 1_x, 2_s);
    // moveToPose(1.5_in, 0_ft, 180_deg, false, 0.6, 0.5_x);

    // driveStraightAtAngle(-0.5_x, 0.08_x, 180_deg, 2_s);
    
    // // Adjust power based on deviation from dead ahead
    // Time init = getTime();
    // Time currentTime = getTime() - init;

    // double deviationFromDesiredDegrees;
    // Speed baseSpeed;
    // Speed speedOffset;

    // while (currentTime.convert(second) < 2) {
    //     currentTime = getTime() - init;

    //     deviationFromDesiredDegrees = chassisAngle().convert(degree); // Positive -> too far right
    //     speedOffset = deviationFromDesiredDegrees * 0.08_x;
    //     baseSpeed = -0.5_x;
    //     setChassisSpeed(
    //         baseSpeed - speedOffset,
    //         baseSpeed + speedOffset
    //     );
    //     delay(20_ms);
    // }

    // setChassisSpeed(0_x, 0_x);
}

