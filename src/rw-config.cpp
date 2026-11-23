#include "rw-config.h"

// ============================================================================
// 5776T PORT MAP
//  1 back distance       8 toggle roller       15 drive RB
//  2 left distance       9 (free)              16 drive LB (rev)
//  3 toggle optical     10 claw distance *     17 drive RC
//  4 front distance     11 lift 1              18 vertical tracker (rev)
//  5 drive LF (rev)     12 IMU                 19 claw twist
//  6 drive RF           13 drive LC (rev)      20 lift 2
//  7 tipping IMU        14 horizontal tracker  21 (free)
//  ADI A: claw piston (LOW = closed, HIGH = open)
//  * clawMotor was also set to port 10 in the LemLib code. Only SoloAWP uses it (storePin/releasePin).
//    Move it to its real port if the motor claw is still on the robot.
// ============================================================================

pros::Controller controller_1(pros::E_CONTROLLER_MASTER);

// Drive: 6 x 11W blue motors, 450 rpm on 2.75" wheels. The first motor in each group gives the
// drive encoder readings (only used if a tracking wheel is turned off below)
pros::MotorGroup left_chassis({-5, -13, -16}, pros::MotorGearset::blue);   // LF, LC, LB
pros::MotorGroup right_chassis({6, 17, 15}, pros::MotorGearset::blue);     // RF, RC, RB

pros::Imu inertial_sensor(12);

// Tracking wheels (a negative port reverses one)
pros::Rotation horizontal_tracker(14);
pros::Rotation vertical_tracker(-18);

// Distance reset sensors. The robot has no right sensor, so it's on a free port
pros::Distance front_sensor(4);
pros::Distance left_sensor(2);
pros::Distance right_sensor(9);
pros::Distance back_sensor(1);

// 5776T mechanisms
pros::Motor clawMotor(-10);
pros::Motor clawTwistMotor(19);
pros::Motor liftMotor1(11);
pros::Motor liftMotor2(20);
pros::Motor rightToggleMotor(8);
// The intake is stubbed out on this robot. It was on -5 in the LemLib code, which is the left front
// drive motor, so it now sits on a free port until it has a real one
pros::Motor intakeBottomLarge(-21);
pros::Optical toggleColorSensor(3);
pros::Distance clawDistanceSensor(10);
// Was declared on port -7 before. An IMU can't take a negative port, so PROS never found it and
// anti-tip never ran. Anti-tip is off by default in src/rw-user.cpp (ANTI_TIP_ENABLED)
pros::Imu tipImu(7);
pros::adi::DigitalOut clawPiston('A', false);

// ============================================================================
// ROBOT GEOMETRY
// ============================================================================

// Track width (LemLib drivetrain: 11.5)
double distance_between_wheels = 11.5;

// motor to wheel gear ratio * wheel diameter (in inches) * pi. 450 rpm from a 600 rpm cartridge
double wheel_distance_in = (450.0 / 600.0) * 2.75 * M_PI;

// PID Constants for movement. These are RW's starting values, NOT the LemLib ones (RW's PIDs work
// on a different scale). Tune them with the "Tuning Test" auton (docs: Guide > Tuning)
// distance_* : Linear PID for straight driving
// turn_*     : PID for turning in place
// heading_correction_* : PID for heading correction during linear movement
double distance_kp = 1.1, distance_ki = 0.1, distance_kd = 7;
double turn_kp = 0.3, turn_ki = 0, turn_kd = 2.5;
double heading_correction_kp = 0.6, heading_correction_ki = 0, heading_correction_kd = 4;

// Both tracking wheels, like the LemLib OdomSensors setup
bool using_horizontal_tracker = true;
bool using_vertical_tracker = true;

// Seen from above with the robot facing forward.
// Horizontal (left-right) wheel: LemLib offset -3.0 (behind center). RW uses + for behind
double horizontal_tracker_dist_from_center = 3.0;
// Vertical (forward-back) wheel: LemLib offset 0.3 (right of center). RW uses + for right too
double vertical_tracker_dist_from_center = 0.3;
// LemLib Omniwheel::NEW_2 = 2.125"
double horizontal_tracker_diameter = 2.125;
double vertical_tracker_diameter = 2.125;

// Distance sensor positions (where the sensor's face is, relative to the robot's center; X + = right,
// Y + = forward). Converted from the OdomDistance setup (1 hole = 0.5"). RW has no per-sensor reading
// offset, so each old readingOffset is added along the direction that sensor faces.
//   front: 11 holes fwd, 4.5 holes right, +0.8" reading  -> ( 2.25,  5.5 + 0.8)
//   left: -9.5 holes fwd, 5 holes left,   +0.2" reading  -> (-2.5 - 0.2, -4.75)
//   back: -7.5 holes fwd, 5.5 holes left, +0.4" reading  -> (-2.75, -3.75 - 0.4)
double front_sensor_offsetX = 2.25;
double front_sensor_offsetY = 6.3;

double left_sensor_offsetX = -2.7;
double left_sensor_offsetY = -4.75;

double right_sensor_offsetX = 0.0;   // no right sensor
double right_sensor_offsetY = 0.0;

double back_sensor_offsetX = -2.75;
double back_sensor_offsetY = -4.15;

// ============================================================================
// ADVANCED TUNING (OPTIONAL)
// ============================================================================

bool heading_correction = true; // Use heading correction when the bot is stationary

// Set to true for more accuracy and smoothness, false for more speed
bool dir_change_start = true;   // Less accel/decel due to expecting direction change at start of movement
bool dir_change_end = true;     // Less accel/decel due to expecting direction change at end of movement

double min_output = 10; // Minimum output voltage to motors while chaining movements

// Maximum allowed change in voltage output per 10 msec during movement
double max_slew_accel_fwd = 24;
double max_slew_decel_fwd = 24;
double max_slew_accel_rev = 24;
double max_slew_decel_rev = 24;

// Prevents too much slipping during boomerang movements
// Decrease if there is too much drifting and inconsistency during boomerang
// Increase for more speed during boomerang
double chase_power = 2;

// ============================================================================
// NEWER SETTINGS (OPTIONAL)
// ============================================================================

// Multiplies the IMU heading. Find yours with the "IMU Scale" auton, 1.0 = no correction
double imu_scale = 1.0;

// End a motion early once the robot has been stuck for this long, in msec. 0 = off.
// Leave it off for moves that push into a goal or wall on purpose
double stall_exit_msec = 0;

// Distance resets: skip readings where the beam hits the wall at a steeper angle than this (degrees)
double distance_reset_max_angle = 30;
// Distance resets: skip readings that would move the position more than this (inches, 0 = off).
// Same job as MAXIMUM_CORRECTION (6") in the old OdomDistance code
double distance_reset_max_jump = 6;
// Distance resets: skip readings that hit the wall this close to a corner (inches, 0 = off).
// For Override, about 18 keeps the corner loaders out of the readings
double distance_reset_corner_margin = 18;

// MCL (off unless startMcl() is called in runPreAutonomous())
int mcl_particles = 200;
double mcl_max_range = 60;
double distance_sensor_delay_msec = 40;

// Multiplies every distance sensor reading (resets and MCL). V5 distance sensors often read 1-3% long.
// Measure: (tape-measured distance to a wall) / (what the sensor reads), at 2-4 ft. 1.0 = no correction
double distance_sensor_scale = 1.0;

// Motion profiles and path following (driveProfiled, turnProfiled, followPath, followPathRamsete)
double drive_kS = 0.5;
double drive_kV = 0;
double drive_kA = 0;
double drive_motor_rpm = 600;          // Blue cartridge
double profile_max_speed = 0;
double profile_max_accel = 100;
double profile_turn_speed = 360;
double profile_turn_accel = 1200;
double path_lookahead = 12;
double path_max_lateral_accel = 120;

// Driver control feel. LemLib arcade had no curve, so this starts linear
double driver_curve = 0;
double driver_deadband = 3;

// Tank/arcade drive, so no strafing
double strafe_distance_in = 0;
