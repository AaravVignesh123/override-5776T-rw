#include "main.h"
#include "autonomous/odometry.hpp"
#include "subsystems/chassis.hpp"
#include "subsystems/intake.hpp"
#include "subsystems/lift.hpp"
#include "subsystems/pneumatics.hpp"
#include "autonomous/autonomous.hpp"
#include "autonomous/odometry.hpp"
#include "autonomous/odomDistance.hpp"
#include "lemlib/api.hpp"
#include "subsystems/claw.hpp"
#include "subsystems/toggle.hpp"

// ----------------------------------------------------------------------------------------------------
// --- Miscellaneous functions ------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------


void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

void pneumaticToggle(int port, bool toggle) {
	if (toggle) {
		pros::c::adi_digital_write(port, HIGH);
	} else {
		pros::c::adi_digital_write(port, LOW);
	}
}

double arrMean(double arr[], int arrLength) {
	double sum = 0;
	double length = arrLength;
	for (int i = 0; i < length; i++) {
        sum += arr[i];
    }
	return sum / length;
}


// ----------------------------------------------------------------------------------------------------
// --- Auton selector ---------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------


enum AutonChars {
	LEFT_AUTO = 'l',
    RIGHT_AUTO = 'r',
    RIGHT_AWP = 'a',
    SKILLS_AUTO = 's',
};

char auton = SKILLS_AUTO;
bool awp = false;
bool noPreload = false;
bool fullScoringPosition = false;

void autonSelector() {
	pros::lcd::set_text(1, "Selected auton:");
	pros::lcd::set_text(2, "Skills");
}


// ----------------------------------------------------------------------------------------------------
// --- Driver Code Variables --------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
bool clawToggle;

struct controllerButton {
	bool pressed = false;
	bool held = false;
	bool released = false;
    bool wasHeld = false;
};

// Pneumatic activation controls
bool scraperToggle;
bool descoreToggle;
bool middleDescoreToggle;

// Controller inputs
pros::Controller controller(pros::E_CONTROLLER_MASTER);
int leftX;
int leftY;
int rightX;
int rightY;
controllerButton xButton;
controllerButton yButton;
controllerButton aButton;
controllerButton bButton;
controllerButton upButton;
controllerButton downButton;
controllerButton leftButton;
controllerButton rightButton;
controllerButton l1Button;
controllerButton l2Button;
controllerButton r1Button;
controllerButton r2Button;

void pollControllerDigital(controllerButton& button, pros::controller_digital_e_t buttonCode) {
    button.held = controller.get_digital(buttonCode);
    button.pressed = button.held && button.held != button.wasHeld;
    button.released = !button.held && button.held != button.wasHeld;
    button.wasHeld = button.held;
}

// Debug
bool holdDrive = false;
int showDebug = 0;

// Macro variables
bool enableMacros = false;
Time currentTime;
enum intakeStates {
    STOPPED = 'S',
    STORE_BALLS = 'H',
    LONG_GOAL = 'L',
    MID_GOAL = 'M',
    LOW_GOAL = 'B'
};
intakeStates intakeCommand = STOPPED;
intakeStates lastIntakeCommand = STOPPED;
double tipAngle = 0.0;
double lastTipAngles[10] = {};  // Mean filter of last 10 angles for noise reduction
int lastTipAnglesSize = sizeof(lastTipAngles) / sizeof(lastTipAngles[0]);
double tipMagnitude = 0.0;
double tipSign = 1.0;
double antiTipCommand = 0.0;
double tipVelocity = 0.0;

// Position recording
lemlib::Pose pose = chassis.getPose();
double poseX;
double poseY;


// ----------------------------------------------------------------------------------------------------
// --- Competition functions --------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------


void initialize() {
	pros::lcd::initialize();
	clawTwistMotor.tare_position();
	allMotors.set_brake_mode_all(pros::MotorBrake::hold);
    // Reset tracking wheel position
	//trackingWheelLR.reset();
	chassis.calibrate();
	//translationTuning();
    //turnTuning();
    tareLiftPosition();
    //liftToPosition(5.3, 100);
    //int32_t modeResult = pros::c::adi_pin_mode(kPneumaticClawPort, OUTPUT);

    while (imu.is_calibrating() || imuTipping.is_calibrating()) {
        pros::delay(10);
    }
    
	pros::delay(250);
	chassis.setPose(lemlib::Pose(0, 0, 0));

    pros::lcd::print(0, "IMU ready");

    const std::int32_t opticalInitResult = initializeToggleColorSensor();
    if (opticalInitResult != PROS_SUCCESS) {
		pros::lcd::print(1, "Optical init failed: %d", opticalInitResult);
    }
	
    clawTwistMotor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	// Initialize pneumatics
    // pros::c::adi_pin_mode(kPneumaticIntakePort, OUTPUT);
    pros::c::adi_pin_mode(kPneumaticClawPort, OUTPUT);

	autonSelector();
	
	delay(1_s);

}

void disabled() {}

void competition_initialize() {}

void autonomous() {
	allMotors.set_brake_mode_all(pros::MotorBrake::hold); // for tighter movements
    tareLiftPosition();
    autonStartTime = getTime();

	switch (auton) {
        case LEFT_AUTO:
            LeftAuto(awp, noPreload);
			break;
        case RIGHT_AUTO:
            RightAuto();
            break;
        case RIGHT_AWP:
            RightAwp();
            break; 
        case SKILLS_AUTO:
            skills();
            break;
	}
}

void opcontrol() {
    Time init = getTime();
    const double tipAngleOffset = imuTipping.get_roll();
    for (int i = 0; i < lastTipAnglesSize; i++) {
        lastTipAngles[i] = 0.0;
    }

    // Set control variables
    clawToggle = false;
	
	// Setting all motors to coast
	allMotors.set_brake_mode_all(pros::MotorBrake::coast);

    // Floor info
    controller.set_text(0, 1, "Floor Up  ");

    chassis.cancelAllMotions();

    // Debug menu
    pros::Task screenTask([&]() {
        while (true) {
            // if (pros::Task::notify_take(true, TIMEOUT_MAX)) break;

            switch (showDebug) {
                case 1: {
            		lemlib::Pose debugPose = chassis.getPose();
            		double angle = chassisAngle().convert(degree);
            		double imuHeading = imu.get_heading();
                    pros::lcd::set_text(0, "LF: " + std::to_string(driveLF.get_position()));
                    pros::lcd::set_text(1, "LC: " + std::to_string(driveLC.get_position()));
                    pros::lcd::set_text(2, "LB: " + std::to_string(driveLB.get_position()));
                    pros::lcd::set_text(3, "RF: " + std::to_string(driveRF.get_position()));
                    pros::lcd::set_text(4, "RC: " + std::to_string(driveRC.get_position()));
                    pros::lcd::set_text(5, "RB: " + std::to_string(driveRB.get_position()));
					//pros::lcd::set_text(4, "IMU hdg: " + std::to_string(imuHeading));
					//pros::lcd::set_text(5, "Wheel:   " + std::to_string(wheelPos));

					//pros::lcd::set_text(6, "Pose x: " + std::to_string(debugPose.x));
					// pros::lcd::set_text(7, "Pose y: " + std::to_string(debugPose.y));
					
					//pros::lcd::set_text(6, "Wheel: " + std::to_string(trackingWheelLRSensor.get_position()));
					
    				//pros::lcd::set_text(7, "Pose: " + std::to_string(debugPose.x) + "," + std::to_string(debugPose.y));
                    
					/*
				    double wheelPos = trackingWheelLRSensor.get_position();
					double wheelVel = trackingWheelLRSensor.get_velocity();
					double imuHeading = imu.get_heading();
					*/

					
					pros::lcd::set_text(6, "Position (ft): (" + std::to_string(chassisX().convert(foot)) + "," + std::to_string(chassisY().convert(foot)) + ")");
                    pros::lcd::set_text(7, "Angle: " + std::to_string(imuHeading) + " deg");
                    break;
                } case 2: {
                    pros::lcd::set_text(0, "Up:    " + std::to_string(upButton.held));
                    pros::lcd::set_text(1, "Down:  " + std::to_string(downButton.held));
                    pros::lcd::set_text(2, "Left:  " + std::to_string(leftButton.held));
                    pros::lcd::set_text(3, "Right: " + std::to_string(rightButton.held));
                    pros::lcd::set_text(4, "X: " + std::to_string(xButton.held));
                    pros::lcd::set_text(5, "Y: " + std::to_string(yButton.held));
                    pros::lcd::set_text(6, "A: " + std::to_string(aButton.held));
                    pros::lcd::set_text(7, "B: " + std::to_string(bButton.held));
                    break;
                } case 3: {
                    pros::lcd::set_text(0, "L1: " + std::to_string(l1Button.held));
                    pros::lcd::set_text(1, "L2: " + std::to_string(l2Button.held));
                    pros::lcd::set_text(2, "R1: " + std::to_string(r1Button.held));
                    pros::lcd::set_text(3, "R2: " + std::to_string(r2Button.held));
                    pros::lcd::set_text(4, "Left stick:  (" + std::to_string(leftX).substr(0, 4) + ", " + std::to_string(leftY).substr(0, 4) + ")");
                    pros::lcd::set_text(5, "Right stick: (" + std::to_string(rightX).substr(0, 4) + ", " + std::to_string(rightY).substr(0, 4) + ")");
                    // pros::lcd::set_text(6, "FB Velocity: " + std::to_string(trackingWheelFBSensor.get_velocity()));
                    pros::lcd::clear_line(6);
                    pros::lcd::clear_line(7);
                    break;
                } case 4: {
                    pros::lcd::set_text(0, "Tipping angle:  " + std::to_string(tipAngle));
                    pros::lcd::set_text(1, "Scraper down:   " + std::to_string(scraperToggle));
                    pros::lcd::set_text(2, "Intake command: " + std::to_string(intakeCommand));
                    pros::lcd::set_text(3, "Right descore:  " + std::to_string(descoreToggle));
                    pros::lcd::set_text(4, "Distance:       " + std::to_string(odomDistanceForward.getDistance().convert(inch)));
                    pros::lcd::set_text(5, "Is Overheating: " + std::to_string(isOverheating()));
                    pros::lcd::set_text(6, "Enable macros:  " + std::to_string(enableMacros));
                    pros::lcd::set_text(7, "Time:           " + std::to_string(currentTime.convert(second)));
                    break;
                } case 5: {
                    pros::lcd::set_text(0, "LF Power Draw: " + std::to_string(driveLF.get_current_draw()));
                    //pros::lcd::set_text(1, "LC Power Draw: " + std::to_string(driveLC.get_current_draw()));
                    pros::lcd::set_text(1, "LB Power Draw: " + std::to_string(driveLB.get_current_draw()));
                    pros::lcd::set_text(2, "RF Power Draw: " + std::to_string(driveRF.get_current_draw()));
                    //pros::lcd::set_text(4, "RC Power Draw: " + std::to_string(driveRC.get_current_draw()));
                    pros::lcd::set_text(3, "RB Power Draw: " + std::to_string(driveRB.get_current_draw()));
                    pros::lcd::set_text(4, "Tipping angle: " + std::to_string(chassisPitch().convert(degree)));
                    pros::lcd::set_text(5, "Untip command: " + std::to_string(antiTipCommand));
                    break;
                } case 6: {
                    pros::lcd::set_text(0, "Dist Forward: " + std::to_string(odomDistanceForward.getDistance().convert(inch)) + " in");
                    pros::lcd::set_text(1, "Dist Left: " + std::to_string(odomDistanceLeft.getDistance().convert(inch)) + " in");
                    pros::lcd::set_text(2, "Dist Claw: " + std::to_string(getClawDistanceMm()) + " mm");
                    pros::lcd::set_text(3, "Dist Backwards: " + std::to_string(odomDistanceBackwards.getDistance().convert(inch)) + " in");
                    pros::lcd::set_text(4, "Current zero = startup");
                    pros::lcd::clear_line(5);
                    pros::lcd::clear_line(6);
                    pros::lcd::clear_line(7);
                    break;
                } case 0: {
                    for (int i = 0; i <= 7; i++) {
                        pros::lcd::clear_line(i);
                    }
                    pros::lcd::set_text(1, "g"); // g
                    break;
                }
            }

            // If you go much faster than this, the brain throws an error because it's redrawing too quickly.
            delay(100_ms);
        }
    });
	/*	
    // Intake task
    pros::Task intakeTask([&] {
		while (true) {
            // if (pros::Task::notify_take(true, TIMEOUT_MAX)) break;

            // Gate controls
            pneumaticToggle(kPneumaticGatePort, l1Button.held);

            // Convert intake command into intake motor powers
            switch (intakeCommand) {
                case STOPPED:
                    stopIntake();
                    break;
                case STORE_BALLS:
                    storeBalls();  // Duplicate intake commands to move the gate at the start to avoid shenanigans
                    if (lastIntakeCommand == MID_GOAL) {
                        clearMiddleSection();
                        delay(CLEAR_MIDDLE_SECTION);
                        storeBalls();
                    }
                    break;
                case LONG_GOAL:
                    scoreBallsUp();
                    if (lastIntakeCommand == MID_GOAL) {
                        clearMiddleSection();
                        delay(CLEAR_MIDDLE_SECTION);
                        scoreBallsUp();
                    }

                    // Score 2 macro
                    if (enableMacros) {
                        delay(TWO_BALLS_INTO_LONG);
                        pros::c::adi_digital_write(kPneumaticGatePort, LOW);
                        scoreBallsDown();
                        delay(200_ms);
                        storeBalls();
                        delay(100_ms);
                    }
                    break;
                case MID_GOAL:
                    scoreBallsMiddle(true);
                    if (lastIntakeCommand != MID_GOAL) {
                        scoreBallsDown();
                        delay(CLEAR_HOOD_SECTION);
                        scoreBallsMiddle(true);
                    }
                    break;
                case LOW_GOAL:
                    if (lastIntakeCommand == MID_GOAL) {
                        clearMiddleSection();
                        delay(CLEAR_MIDDLE_SECTION);
                    }
                    scoreBallsDown();
                    break;
            }

            // Don't update lastIntakeCommand if the last command was a stopped intake (the balls haven't moved)
            // lastIntakeCommand is used to know if we should clear the middle section of a ball or not when scoring elsewhere
            // or whether we should clear the hood area of a ball or not when scoring into the middle goal
            if (intakeCommand != STOPPED) {
                lastIntakeCommand = intakeCommand;
            }

            delay(10_ms);
		}
  	});
	*/




	// Main loop
	while (true) {
		currentTime = getTime() - init;
		pose = chassis.getPose();

		// Poll controller analog
		leftX = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);
		leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
		rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
		rightY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);

		// Poll controller digital
        pollControllerDigital(xButton, pros::E_CONTROLLER_DIGITAL_X);
        pollControllerDigital(yButton, pros::E_CONTROLLER_DIGITAL_Y);
        pollControllerDigital(aButton, pros::E_CONTROLLER_DIGITAL_A);
        pollControllerDigital(bButton, pros::E_CONTROLLER_DIGITAL_B);
        pollControllerDigital(upButton, pros::E_CONTROLLER_DIGITAL_UP);
        pollControllerDigital(downButton, pros::E_CONTROLLER_DIGITAL_DOWN);
        pollControllerDigital(leftButton, pros::E_CONTROLLER_DIGITAL_LEFT);
        pollControllerDigital(rightButton, pros::E_CONTROLLER_DIGITAL_RIGHT);
        pollControllerDigital(l1Button, pros::E_CONTROLLER_DIGITAL_L1);
        pollControllerDigital(l2Button, pros::E_CONTROLLER_DIGITAL_L2);
        pollControllerDigital(r1Button, pros::E_CONTROLLER_DIGITAL_R1);
        pollControllerDigital(r2Button, pros::E_CONTROLLER_DIGITAL_R2);

		// Macros
		enableMacros = aButton.held;
        if (enableMacros && r2Button.pressed) {
            farSideMatchloader(0_in, 0_in);
        }
        if (enableMacros && upButton.pressed) {
            scoreFarSide(0_in, 0_in, 2);
        }
        if (enableMacros && downButton.pressed) {
            scoreFarSide(0_in, 0_in, 4);
        }
        if (enableMacros && r1Button.pressed){
            scoreFarSide(0_in, 0_in, 3);
        }



		// Claw controls
		// after pollControllerDigital(...) calls
		if (yButton.pressed) {
            clawToggle = !clawToggle;
        }
        pneumaticToggle(kPneumaticClawPort, clawToggle);

        // Toggle controls
        if (!enableMacros && (r1Button.held || isNonAllianceColorDetected())) {
            toggleRollers();
		} else {
            //setMotorSpeed(leftToggleMotor, 0_x);
            setMotorSpeed(rightToggleMotor, 0_x);
        }

        // Auto-grab stays active and closes the claw while an object is in range.
        if (autoGrab()) {
            clawToggle = false;
            pneumaticToggle(kPneumaticClawPort, clawToggle);
        }

        // Lift controls
		if (bButton.held) {
			liftDown();
		} else if (rightButton.held) {
			liftUp();
		} else {
			setMotorSpeed(liftMotor1, 0_x);
			setMotorSpeed(liftMotor2, 0_x);
		}

        // Debug things
		if (xButton.pressed) {
            showDebug = (showDebug + 1) % 7;
		}

        // Anti-tip protection

        tipAngle = std::remainder(imuTipping.get_roll() - tipAngleOffset, 360.0);
        tipMagnitude = std::abs(tipAngle);
		tipSign = std::signbit(tipAngle) ? 1 : -1;
        tipVelocity = (tipAngle - arrMean(lastTipAngles, lastTipAnglesSize)) * tipSign * 10.0;

        // Disable anti-tip for the final 30 seconds of driver control.
		if (tipMagnitude > 10 && currentTime < 75_s) {
            double correctionMagnitude = std::clamp(
                (tipMagnitude - 10.0) * 2.0 + std::max(0.0, tipVelocity) * 0.1,
                0.0,
                80.0
            );
            antiTipCommand = tipSign * correctionMagnitude;

            leftY = static_cast<int>(antiTipCommand);
		} else {
			antiTipCommand = 0;
		}

		// Shift all anti-tip angle values over by one
		for (int i = lastTipAnglesSize - 1; i > 0; i--) {
			lastTipAngles[i] = lastTipAngles[i - 1];
		}
		lastTipAngles[0] = tipAngle;

        

		// Tank controls
        //chassis.tank(leftY, rightY);
        // 2-stick arcade controls
        // chassis.arcade(leftY, rightX);
		chassis.arcade(leftY, rightX / 2);

		delay(10_ms);



        /*
		if (rightButton.held){
			storePin();
			intakePins();
		} else if (leftButton.held) {
			releasePin();
			outtakePins();
		} else {
			setMotorSpeed(clawMotor, 0_x);
			stopIntake();
		} */

		/* // Intake controls
		if (r1Button.held) {
            intakePins();
		} else if (r2Button.held) {
			outtakePins();
		} else {
			stopIntake();
			intakeCommand = STOPPED;
		} */
		
		/* if (l1Button.held) {
			scoringReadyPosition();
		} else if (l2Button.held) {
			restPosition();
		} else if (xButton.held) {
			clawTwistMaxPosition();
		} */
			
		/*	
		if (l1Button.held) {
			if (xButton.held) {
				fullScoringPosition = true;
			}

			if (fullScoringPosition) {
				scoringReady2();
			} else {
				scoringReadyPosition();
			}
		} else if (l2Button.held) {
			fullScoringPosition = false;
			restPosition();
		} else {
			fullScoringPosition = false;
		}
			*/
		

		// Scraper toggle
		/*
		if (!enableMacros && bButton.pressed) {
			scraperToggle = !scraperToggle;
			pneumaticToggle(kPneumaticScraperPort, scraperToggle);
		}
		*/

		// Descore toggle
		/*
        descoreToggle = rightButton.held;
        pneumaticToggle(kPneumaticDescorePort, descoreToggle);

        // Middle descore toggle
        middleDescoreToggle = l2Button.held;
        pneumaticToggle(kPneumaticMiddleDescorePort, middleDescoreToggle);
		*/


        // if (auton == SKILLS) {
        //     pneumaticToggle(kPneumaticDescorePort, true);
        // }

        
	}
}
