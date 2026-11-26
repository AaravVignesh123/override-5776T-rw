# LemLib → RW-Template port (5776T)

This repo is `override-5776t-lemlib` moved onto [RW-Template](https://github.com/richardbwang/RW-Template) 4.1.0 (PROS). LemLib, the units library and liblvgl/LLEMU are gone. RW now runs the drive, odometry, distance resets, auton selector and simulator.

## Where things are now

| Before (LemLib) | Now (RW) |
|---|---|
| `subsystems/chassis.cpp`, `autonomous/odometry.cpp` (ports, PIDs, wrappers) | `src/rw-config.cpp`: every port on the robot plus RW settings |
| `autonomous/odomDistance.cpp` (`OdomDistance`, `ForceToWall`) | `resetPositionFront/Left/Back()` through `distanceReset()` in `src/rw-autons.cpp` |
| `autonomous/autonomous.cpp` | `src/rw-autons.cpp` |
| `main.cpp` `initialize()` / `opcontrol()` / `autonomous()` | `src/rw-user.cpp` `runPreAutonomous()` / `runDriver()` / `runAutonomous()` |
| `char auton = SKILLS_AUTO` | RW selector on the Brain screen, or the controller arrows while disabled (it remembers the last pick) |
| LLEMU debug pages | Controller screen. X cycles pose / distance sensors / mechanisms / drive health |
| `units` (`_ft`, `_in`, `_x`, `_ms`) | Plain numbers: inches, degrees, ms, volts (12 = full) |

`src/subsystems/` still holds the claw, lift, toggle, pneumatics and intake code, with the same function names. Only the speeds changed, from `_x`/`_power` to `motor.move(-127..127)`.

## How the autons were converted

Each route keeps its tuned **local** numbers. `setLocalOrigin(x, y)` says where local (0, 0) is on the field, and `fx()`/`fy()` convert positions, so RW's field-centered resets and MCL work. The mapping table is at the top of `src/rw-autons.cpp`.

## Before running on the field

1. **Tune the PIDs.** The RW defaults are in place, not the LemLib values, because RW uses a different scale. Run **Tuning Test** and follow RW's docs (Guide > Tuning). In the simulator with the defaults, turns over about 100° don't finish inside the old 800 ms turn timeouts. Either raise those timeouts or tune `turn_*` until the turns settle in time.
2. **Check tracking-wheel directions.** Run **Odom Test**, or push the robot by hand: forward should raise `y_pos` and moving right should raise `x_pos` (controller page 0). If one goes the wrong way, flip the sign of that rotation port.
3. **Measure each starting spot.** Tape from two walls to the robot's center and put the real numbers into each route's `setLocalOrigin()`. The current values are estimates from the old per-sensor `setOriginPosition()` calls, which disagreed with each other by up to about 6".
4. **Check the distance sensor offsets** with **Dist Reset Test**: put the robot's center 48" from the left and back walls. Then measure `distance_sensor_scale` and run **IMU Scale** to get `imu_scale`.
5. **Watch the terminal.** Every reset prints `[reset name] old -> new` or `skipped`. A skipped reset changes nothing, because `distance_reset_max_jump` is 6", the same limit as the old `MAXIMUM_CORRECTION`.

## Problems found in the old code (decide what to keep)

- **Port 10:** `clawMotor(-10)` and the claw distance sensor were both on port 10. Only `SoloAWP` uses `clawMotor`.
- **Intake on port -5:** `intakeBottomLarge` was on -5, which is the left front drive motor. It's on a free port (-21) now, since the intake is stubbed out.
- **Tipping IMU on port -7:** an IMU can't take a negative port, so PROS never found it and anti-tip never ran. It's on port 7 now, and anti-tip is off by default (`ANTI_TIP_ENABLED` in `src/rw-user.cpp`). Before turning it on, check that the correction pushes the right way: it takes over the throttle.
- **Lift left running in `SoloAWP`:** three of the lift-up loops never stopped the lift. That behavior is kept and marked with comments.

## Things that behave differently

- **Corner margin:** `distance_reset_corner_margin = 18` (RW's Override value) skips the front reset in `matchloadingRight()`, because the robot is too close to the corner loader there. The old code used that reset. Lower the margin to about 10 if that reset was reliable.
- **Scoring macros:** `scoreFarSide()` now waits for its last move before driver control resumes, so the macro and the joysticks don't fight over the drive.
- **Left standing pins:** the standing-pin part of `LeftAuto` (commented out in LemLib) is converted and compiles. It runs only when `LEFT_RUN_STANDING_PINS = true`.

## Simulator

```bash
sh sim/run-sim.sh "Skills" --start -10.91,10.22,44 --time 60
```

This writes a replay to `sim/out/Skills.html`. All five routes run to the end in the simulator. The replays are only as accurate as the start origins and PIDs.

## Recording a route

In driver control, press **D-pad Left** to start recording, drive the route with the joysticks, then press **D-pad Left** again to stop and export. The recorder samples odometry positions and exports a `recordedRoute()` waypoint function; it does not record mechanism actions. The controller screen shows the recording state and waypoint count.

With a writable microSD card, the generated code is saved as `/usd/rw-recorded-route.cpp`. Without one, it prints the code to the PROS terminal. Copy the function into `src/rw-autons.cpp`, declare it in `include/rw-config.h`, and add it to the selector in `src/rw-user.cpp` as indicated in the generated comments. The route uses field-centered odometry coordinates and assumes the robot starts in the same physical position and heading. Test and tune it before competition use.
