/**
 * \file main.h
 *
 * Contains common definitions and header files used throughout the project.
 * Drivetrain, odometry and autons come from RW-Template (include/rw/), set up in
 * src/rw-config.cpp, src/rw-user.cpp and src/rw-autons.cpp.
 */

#ifndef _PROS_MAIN_H_
#define _PROS_MAIN_H_

#define PROS_USE_SIMPLE_NAMES

#include "api.h"

#ifdef __cplusplus
extern "C" {
#endif
void autonomous(void);
void initialize(void);
void disabled(void);
void competition_initialize(void);
void opcontrol(void);
#ifdef __cplusplus
}
#endif

#endif  // _PROS_MAIN_H_
