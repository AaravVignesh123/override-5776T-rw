# V5RC 2026-2027 "Override": research notes for 5776T

Compiled 2026-11-22. The current rules are **Game Manual v2.1** (published Oct 8, 2026, in effect from Oct 15, 2026). The next scheduled update is v2.2 on Dec 3, 2026.

Confidence labels used throughout:
- **[manual]**: from the official manual (v2.1 online, or the v1.1 PDF where v2.1 did not change it).
- **[Q&A]**: from an official Q&A ruling.
- **[data]**: from VEX's public skills-standings data.
- **[3rd-party]**: from a community or third-party source.
- **[inferred]**: my own reading or calculation. Check it on a real field.
- **unverified**: I could not confirm it.

All quotes are under 15 words. Everything else is paraphrased.

> **Heads-up about the codebase:** several names in this repo come from last season's game, Push Back (2025-26): `LONG_GOAL`, `MIDDLE_GOAL`, "matchloader", scraper and descore. Override has **no long goals**. It does have:
> - Alliance Goals (3.25" tall)
> - Short neutral Goals (5.8")
> - One tall neutral Goal in the Midfield (8.7")
> - Loaders (not "matchloaders")
>
> The Override-specific parts of the code are the Toggle rollers, the pin claw, "standing pins" and the alliance-goal routes. Those match the game as described below.

---

## 1. Game overview

### 1.1 Match format and timing [manual]
| Item | Value |
|---|---|
| Field | 12' x 12', 36 foam tiles, 2 v 2 (red and blue Alliances) |
| Autonomous Period | 0:15 |
| Driver Controlled Period | 1:45 |
| Endgame | The last 10 s of a Head-to-Head Match ("king of the hill" for the Midfield) |
| Driving Skills / Autonomous Coding Skills | 1:00 each, one Robot, starting on the red side |
| Scoring evaluated | After the match ends: 5 s after 0:00, or once everything is at rest, whichever comes first (SC1) |

### 1.2 Field contents [manual]
- **56 Cups**, about 3.15" in diameter and 6.5" tall. Each Cup has one **transparent half** and one **opaque half**.
  - 20 are Match Loads (10 per Alliance).
  - 24 start on the field gray side up.
  - 12 start on the field clear side up.
- **63 Pins**, about 1.6" in diameter and 6.5" tall. Each Pin has two halves, and each half is red, blue or yellow.
  - 4 red/blue. All start on the field.
  - 20 red/yellow: 2 Preloads, 10 Match Loads, 8 on the field.
  - 20 blue/yellow: 2 Preloads, 10 Match Loads, 8 on the field.
  - 19 yellow/yellow: 2 Match Loads, 17 on the field.
- **9 Goals** (octagonal):
  - 4 Alliance-colored (2 red, 2 blue), 3.25" (82.5 mm) tall.
  - 4 short neutral (black), 5.8" (146.5 mm) tall.
  - 1 tall neutral in the center, 8.7" (222.7 mm) tall.
- **4 Toggles**: triangular prisms, 25.8" long, with faces about 2.05" wide. One is mounted at the middle of each wall. The three faces are red, blue and yellow.
- **4 Loaders**: two per Alliance, in the corners next to that Alliance's station. Each Loader has a taped **Load Zone** around it.
- **Quadrants**: four triangular regions bounded by the two diagonals. Each contains one Alliance Goal, one short neutral Goal and one Toggle. A Quadrant is "red" or "blue" according to its Alliance Goal.
  - Tournament Manager calls them Red 1, Red 2, Blue 1 and Blue 2.
  - Red 1 and Blue 1 are the Quadrants adjacent to their own Alliance Station [Q&A 3180].
- **Midfield**: the square central area inside the white-tape square, with infinite height. The tall Goal sits in the center.
- **Autonomous Line**: a pair of white tape lines running diagonally across the field and around the Midfield, plus the space between them.
- **AprilTags** are on the Goals. The layout is the same on Head-to-Head and Skills fields [Q&A 3294].

### 1.3 Coordinates (field-centered, inches; +x toward the blue Alliance Station; +y toward the "top" of manual figure FO-2)

**Sources for the coordinates:**
- The VEXcode VR "V5 26-27 Override" playground lists element positions in mm, with (0,0) at the field center. The **mm** column is that data.
- The **in** column is mm / 25.4.
- The VR playground puts tiles at 600 mm, but a real tile is 24" (609.6 mm). On a real field, expect positions at multiples of about **24" / 48" (likely; unverified)**.
- Assignment to Quadrants and the orientation were checked against manual Figures FO-2, SG-1 and RSC4-1 (v1.1 PDF) [inferred from figures].
- Walls: your measured value of ±70.25" is consistent with these numbers. The VR toggle centers sit at ±70.1" (on the perimeter).

| Element | mm (VR) | inches | Quadrant / side |
|---|---|---|---|
| Tall neutral Goal | (0, 0) | (0, 0) | Midfield |
| **Red Alliance Goal (Red 1 / left Quadrant)** | (-1200, -600) | (-47.2, -23.6) | red |
| **Red Alliance Goal (Red 2 / bottom Quadrant)** | (-600, -1200) | (-23.6, -47.2) | red |
| Blue Alliance Goal | (600, 1200) | (23.6, 47.2) | blue (top) |
| Blue Alliance Goal | (1200, 600) | (47.2, 23.6) | blue (right) |
| Short neutral Goal, left Quadrant | (-1200, 600) | (-47.2, 23.6) | red side |
| Short neutral Goal, bottom Quadrant | (600, -1200) | (23.6, -47.2) | red side |
| Short neutral Goal, top Quadrant | (-600, 1200) | (-23.6, 47.2) | blue side |
| Short neutral Goal, right Quadrant | (1200, -600) | (47.2, -23.6) | blue side |
| Toggle, left wall (red Q) | (-1780, 0) | (-70.1, 0) | red |
| Toggle, bottom wall (red Q) | (0, -1780) | (0, -70.1) | red |
| Toggle, top wall (blue Q) | (0, 1780) | (0, 70.1) | blue |
| Toggle, right wall (blue Q) | (1780, 0) | (70.1, 0) | blue |
| Red Loaders (left wall corners) | (-1740, ±1490) | (-68.5, ±58.7) | VR lists red only; blue is presumably the mirror (68.5, ±58.7) [inferred] |

**Starting objects** (20 positions from VR; contents identified from Fig. FO-2) [inferred]:
- **Midfield corners** (0, ±23.6) and (±23.6, 0): a **red/blue Pin standing in a clear-up Cup**, 4 in total. These sit on the Midfield/Autonomous-Line tape.
- **The y = x diagonal** (+,+ and -,- only):
  - (±23.6, ±23.6) and (±47.2, ±47.2): a **yellow/yellow Pin standing in a clear-up Cup**.
  - These are probably the "standing pins" in your code.
- **The y = -x diagonal (the Autonomous Line)**: (-47.2, 47.2), (-23.6, 23.6), (23.6, -23.6) and (47.2, -47.2).
  - Each is a "cross" cluster: a clear-up Cup ringed by **4 lying Pins** (2 red/yellow and 2 blue/yellow, yellow ends pointing inward).
  - These 16 Pins and 4 Cups, plus the 8 Midfield objects, are the **28 objects that start on the Autonomous Line**. Either Alliance may take them in auton (SG7b).
- **Along each wall**, at (±23.6, ±68.7) and (±68.7, ±23.6): a **yellow/yellow Pin in a Cup flanked by 2 empty Cups** (8 groups, 24 Cups).
- **Each of the 5 neutral Goals** appears to start with **one yellow/yellow Pin already Placed** (Fig. FO-2).
  - This makes the counts add up: 8 wall + 4 diagonal + 5 in Goals = 17 yellow/yellow.
  - Alliance Goals start empty.
  - In Skills, **all Goals start empty** (RSC4c).
- **Midfield shape**: from the figures, a square rotated 45°, with its corners at about the four red/blue Pins, so roughly |x|+|y| ≤ 24". **Exact tape dimensions unverified.**
- **Autonomous Line**: a double tape along the y = -x diagonal (top-left corner to bottom-right corner) that wraps the Midfield.
  - **Red side is x + y < 0.** The red station is on the left (x ≈ -70) wall. Both red Quadrants (left = Red 1, bottom = Red 2) and both red Toggles are on the red side.
- **Robot start (Fig. SG-1)**: each Robot starts in its own Quadrant (two Robots may not share one), touching tiles and the perimeter on its own side.
  - The figure shows each Robot against its Quadrant's wall, right next to that wall's Toggle.
  - So a "spin the Toggle first" opening is the natural start. Your `LeftAuto` and `RightAuto` already do this.

**Symmetry, so you can write routes once** [inferred]:
- **Red 1 ↔ Red 2** is a reflection across y = x: (x, y) → (y, x).
  - With compass heading (0° = +y, clockwise positive), θ → 90° − θ.
- **Red ↔ Blue** is a 180° rotation: (x, y) → (−x, −y), θ → θ + 180°.

### 1.4 Scoring [manual v2.1]
| Action | Points |
|---|---|
| Autonomous Bonus | **12** (6 each on a tie, including 0-0) |
| Each Scored Alliance-colored Pin **Half** | **5** |
| Each Scored yellow Pin **Half** (only when Owned) | **10** |
| Each Robot ending in the Midfield | **8** |

How the rules work:
- **Placed (SC2)**:
  - A Pin is Placed if it is nested with a Goal, or with a Cup that is itself nested with another Placed Pin.
  - Each Goal half or Cup half nested with that Pin may hold **at most one Pin half**.
  - A Cup is Placed if it is nested with a Placed Pin.
  - Robot contact does not cancel Placed status.
  - **Consequence:** you cannot stack a Pin directly on a Pin. Stacks alternate Pin, Cup, Pin, Cup and so on.
- **Scored (SC3)**: each Pin half that is **fully visible** scores for its color.
  - A half nested in a **Goal** or in the **opaque half of a Cup** does not score.
  - Partial coverage by a gray Cup also means not Scored [Q&A 3175].
- **Visible halves per stack** [inferred]:
  - Each Pin has 2 halves. The Goal hides one, and each Cup's opaque half hides one more when a Pin sits in it.
  - So a stack of N Pins usually shows about **N visible halves**.
  - **Cup orientation decides which half shows**. With a red/yellow Pin you can choose a 5-point red half or a 10-point yellow half.
  - A claw that can **twist or flip the Pin** is therefore worth points.
- **Toggle set (SC4)**:
  - The Toggle must be fully seated, with one face flat and parallel against its mounts at rest.
  - It must **not be touching any Robot**. The metal brackets are not part of the Toggle [Q&A 3171].
  - Otherwise it counts as yellow/neutral. Only 3 discrete set states exist.
- **Yellow ownership (SC5)**:
  - In a Quadrant: the Alliance whose color the Quadrant's Toggle shows owns the yellow halves.
  - In the Midfield Goal: the Alliance with **more Robots in the Midfield at the end of the match** owns them. A tie means nobody owns them.
- **Midfield (SC6)**: any part of the Robot inside the Midfield volume at the end counts.
- **Autonomous Bonus (SC7)**:
  - Evaluated immediately after auton. Midfield-position scoring (Robots in the Midfield, Midfield yellow ownership) is **excluded**.
  - Any auton Violation gives the Bonus to the opponent. If both Alliances violate, no Bonus is awarded.
- **Point swings** [inferred]:
  - About 62% of all Pin halves are yellow (78 of 126) [3rd-party: Spartan Design].
  - Flipping a Toggle can swing a quadrant by tens of points. The GDC letter in the manual says swings of this size are intended.

### 1.5 Autonomous Win Point (SC8, v2.1) [manual]
To earn it, end the Autonomous Period with all of the following, and with **no Violations in auton**.

**Standard events:**
1. At least **6 Pin Halves Scored for your Alliance**.
2. At least **2 Goals**, each containing at least **2 Pin Halves Scored for your Alliance**.
3. **Neither Robot touching the Field Perimeter.**

**Events that qualify directly to Worlds** (Signature Events, Event Region Championships):
- **7 halves** and **3 Goals** (each with at least 2 halves), plus no perimeter contact.
- The Worlds criteria themselves may be tweaked in a later update.

Details:
- Halves or Goals in **Quadrants on the opposing side** of the Autonomous Line do not count.
- **Owned yellow halves count** [Q&A 3179]. So setting your Toggle before auton ends turns the yellow halves in your Quadrant into AWP halves.
- Halves must be **Scored**, not merely Placed [Q&A 3154].
- **Yellow** halves in the Midfield Goal **never count for AWP or the Bonus**, because ownership is decided only at match end. **Red/blue** halves in the Midfield Goal **do count** [Q&A 3282].
- **History:** v1.1 required 7 Pins and 3 Goals at every event. v2.0 (Sept 3, 2026) lowered the standard criteria to 6 and 2 [3rd-party Spartan Design summary; consistent with the v2.1 text].
- **Ranking**:
  - Winning the Autonomous Bonus earns 10 Autonomous Points (AP); a tie earns 5 each.
  - A win is worth 2 Win Points.
  - AWP is an extra Win Point.
  - SP (Strength of Schedule Points) equals the losing Alliance's score.

### 1.6 Robot Skills (Section 3) [manual v2.1]
- **Start**: legal start in the **Quadrant adjacent to the red Alliance Station** (Red 1 / left Quadrant). The Preload is one red/yellow Pin.
- **Match Loads**: **3 red/yellow Pins, 4 blue/yellow Pins and 7 Cups**.
  - The drive team may introduce them **at any time**, including during Autonomous Coding Skills, but **only through the red Loaders** [RSC2, Q&A 3196].
  - A Loader takes a single Pin, a single Cup, or a nested Cup + Pin. The Robot removes them through the bottom opening.
- **Field setup**: all Goals start empty. The GPS strip must be installed for Autonomous Coding Skills.
- **Freedom**: the Robot may add or remove objects on any Goal. Removing objects from the field is not a Violation, but they are not returned.
- **Skills scoring (RSC3)**:
  - Red halves score only in **red Quadrants or the Midfield**. Blue halves score only in **blue Quadrants or the Midfield**.
  - Yellow halves score only when Owned:
    - In a Quadrant, the Toggle must show that **Quadrant's own color**, so blue Quadrants need blue Toggles even in a red-only run.
    - In the Midfield Goal, a Robot must end in the Midfield.
  - 5 / 10 / 8 points, the same as in matches.
  - The old "Pin touching a Robot doesn't score" clause was **deleted** in the Sept 3 update [Q&A 3199].
- **No Endgame in Skills.** You may place on the Midfield Goal at any time [Q&A 3242].
- **Skills Stop Time** is an opt-in tiebreaker (RSC5).

### 1.7 Key game rules (v2.1) [manual unless noted]
- **SG1 Start**:
  - 18"x18"x18".
  - Touching only your Preload, not Goals, Loaders, Load Zones or Toggles.
  - Not touching another Robot, and **not sharing a Quadrant**.
  - Stationary.
  - Touching tiles and perimeter **on your side** of the Autonomous Line.
- **SG2 / SG3 Expansion**:
  - Never more than a 24"x24" footprint, including any sideways swing from lifts.
  - Never more than **50"** tall.
- **SG4**: no intentional removal of Scoring Objects from the field. Objects resting on top of the perimeter count as out.
- **SG5**: one Preload Pin per Robot (red/yellow for red, blue/yellow for blue).
- **SG6 Possession**: at most **1 Pin and 1 Cup** at a time.
  - If over, stop and shed the extra. Until you do, you cannot touch Toggles, Goals or objects, and you get no Midfield points.
  - Per a 3rd-party summary, v2.1 broadened "Possession" to include objects supported by a Possessed object, and v2.0 removed "Plowing" as a defined status. **Verify in v2.1 Appendix B.**
- **SG7 Autonomous**:
  - No contact with tiles, objects or field elements on the opponent's side.
  - The 28 objects starting on the line are fair game.
  - Do not push objects across the line.
  - Contacting an opponent Robot (unless it is engaging the line or Midfield) or objects on their side gives them the **Bonus and the AWP**.
  - Accidental contact is a Minor Violation. Intentional or egregious contact is a Major Violation and a DQ [Q&A 3262].
- **SG8**: engage the Midfield or Autonomous Line in auton "at your own risk". Contact there is usually not penalized.
- **SG9**: no direct or indirect interaction with **opposing Alliance Goals** (Major if you add or remove anything). You may remove objects from your **own** Alliance Goal [Q&A 3249].
- **SG10**: no removing **Placed** objects from **neutral** Goals.
  - Match-affecting value is 10 points per Pin for 1-3 Pins, 50 for 4 or more (v2.0+ Violation Notes).
  - Unplaced objects sitting on a Goal may be removed [Q&A 3153].
- **SG11 Match Loads**:
  - Driver Controlled Period only in matches.
  - Only through your own Alliance's Loaders, by top or back.
  - Only Robots of that color remove them, through the bottom.
- **SG12 Endgame** (changed in v2.0):
  - Robots **may not Place objects on the Midfield Goal** during the Endgame.
  - Expect vigorous contact in the Midfield. Incidental damage there is not a GG14 Violation.
  - The v1.1 height soft-limit was **removed**.
- **SG13 Load Zones** (Driver Controlled only):
  - A Robot partly inside its own Load Zone is protected.
  - You may only pass through the opponent's Load Zone momentarily.
  - Do not pile objects there, and do not add to or remove from the opposing Loaders.
- **Toggle jamming**: wedging a Toggle (for example with C-channel) is not automatically a GG9 Violation. The GDC is watching this [Q&A 3135].
- **Robot (R10/R11)**:
  - 88 W motor budget.
  - Drivetrain capped at 55 W, with no power takeoff from it (v2.1 R11) [3rd-party summary of v2.1; the quick reference confirms that R10/R11 exist].
- **Upcoming R28 change**: you will be allowed to trim the plastic mounting flanges on Distance, Optical and Bumper sensors. This is effective immediately, with formal text in v2.2 on Dec 3 [Q&A 3257].

### 1.8 Manual timeline
| Version | Date | Highlights |
|---|---|---|
| 0.1 | Apr 27, 2026 | Release |
| 0.2 | Jun 4 | Pin Placed/Scored clarifications. AWP counts **Scored** Pins. |
| 1.0 | Jul 2 | SG4/SG9/SG10 revisions, Endgame soft height limit, SG13 Load Zones added |
| 1.1 | Aug 6 | SC7 (Midfield excluded from auton), RSC2 (skills match loads any time), Midfield definition fix |
| 2.0 | Sep 3 (effective Sep 10) | Standard AWP lowered to 6 halves / 2 Goals. Worlds-qualifier AWP set at 7 / 3. SG12 changed to "no placing on Midfield Goal". SG10 point values. RSC3 contact clause removed. [3rd-party summary plus Q&As] |
| 2.1 | Oct 8 (effective Oct 15) | SC8 wording in "Pin Halves". R11 drivetrain 55 W. Possession definition widened. [3rd-party summary] |
| 2.2 | Dec 3 (scheduled) | Minor. R28 sensor flanges. |
| 3.0 / 4.0 | Jan 28 / Mar 25, 2027 | Possible gameplay changes |

---

## 2. Meta: what top teams are doing

### 2.1 Skills standings, Override season (High School, pulled 2026-11-22) [data]
| Rank | Team | Combined | Programming | Driver | Event (date) |
|---|---|---|---|---|---|
| 1 | 11111Y (Shanghai) | **286** | **163** | 123 | VE-V5-26-66072 (Nov 20) |
| 2 | 2429A (CA) | 266 | 128 | 138 | One World Signature @ UC Berkeley (Nov 21) |
| 3-5 | 8737M ROKKR (CA), 63780D (GA), 68678M (Taiwan) | 256 | 128 | 128 | Smoky Mountain Forge Sig (Oct 9); GATSA (Nov 13); Berkeley Sig |
| 6-8 | 1698Z, 66799T, 96Z Ctrl Z | 256 | 118 / 118 / 108 | 138 / 138 / 148 | Berkeley Sig; Oct 25; Highlander Summit Sig (Sep 25) |

- Highest driver scores: **148** (96Z, 7899G, 54001A).
- Highest MS combined: 246 (5798V and 10606A). MS driver peaks at 158 (61B).
- 2,214 HS teams have a skills score. The **median combined is 43** and the median programming is 8. Programming is still weak across the field.
- **5776T today: rank 399, combined 98 (programming 10, driver 88).** 5776A has 108 (driver only), and 5776V has 78 (programming only).
- **Pattern** [inferred]: nearly every top score ends in 8 (128, 118, 108...). Top runs **finish parked in the Midfield** (+8). The rest is almost all 10-point yellow halves. For example, 128 = 8 + 12 owned yellow halves.
- A YouTube video titled "226 Official Skills" for 95071V exists. Its description was not readable.

### 2.2 Notable events so far (results pages were not readable; scores only)
- Highlander Summit Signature, Newark NJ (Sep 25-27).
- Smoky Mountain Forge Signature, TN (Oct 9-10).
- One World Signature @ UC Berkeley (Nov 21-22).
- Several Chinese "VE-" events produce the top skills scores.
- Tournament champion and Excellence winners: **unverified** (the event pages showed pre-event text only).

### 2.3 Robot archetypes [3rd-party, low confidence]
- **Precision stacker**: double-reverse four-bar (DR4B) lift, Pin claw, and a funnel intake that centers the Pin before lifting. This is the closest to 5776T's design.
- **Cycle machine**: cascade lift with a guided carriage. Forum posters claim a DR4B is much faster to full height than a cascade (about 0.5 s against about 3 s), while a cascade is lighter and more compact.
- **Utility partner**: a short pivot arm that handles Toggles and defends.
- **Toggle mechanisms**: bumper push, manipulator press, or a dedicated roller or flipper. The roller/flipper costs a motor out of the 88 W.
  - 569C's public code has a dedicated "toggler" motor. Several public PROS/LemLib repos add a single distance-sensor wall reset.
- **Strategy consensus** (forum, Spartan Design):
  - Toggle control is the largest lever.
  - Common autos are "toggle, then 2 on the Alliance Goal, then 1 on neutral".
  - Accumulator designs are pointless because of the 1 Pin + 1 Cup limit.
  - The Endgame is a pushing fight, not a climb.

---

## 3. Autonomous routes catalog

### 3.1 Assumptions
- **Field and robot**:
  - Coordinates are red-side and field-centered, from 1.3.
  - "Unit" means a standing Pin-in-Cup (the yellow/yellow on the y = x diagonal, or a wall stack). Taking it as one piece is legal (1 Pin + 1 Cup).
  - Dropping a unit on a stack nests the Cup over the Pin below and adds one visible half [inferred].
- **Points**:
  - Points assume yellow-up in a Quadrant whose Toggle you have set.
- **Timing and verification**:
  - Timings are estimates for a 450 rpm drive with a claw and lift. **Not measured.**
  - None of these routes come from a top team's published code. They are built from the rules and layout, plus the shape of 5776T's existing routes. **Treat them as plans to test.**

### 3.2 Red 1 / left Quadrant: "Toggle + Alliance stack" (AWP half). Your `LeftAuto` is this.
1. **0-1.0 s**: spin the left Toggle (-70, 0) to your color. Stop when the optical sensor sees alliance hue. Back off so you are not touching it (SC4b).
   - Bonus [inferred]: the yellow Pin preplaced in the left neutral Goal (-47, +24) becomes Owned. That is **+10** and **1 AWP half** with no further work.
2. **1-3.5 s**: drive to the Red Alliance Goal (-47, -24). Lift (async), then drop the Preload **yellow-up**: **+10**, AWP half 2.
3. **3.5-7.5 s**: claw the diagonal unit at (-23.6, -23.6) or the wall unit at (-68.7, -23.6). Return and drop it on the Alliance Goal: **+10**. This Goal now has 2 halves, the first AWP Goal.
4. **7.5-12 s**:
   - For a quals partner split: take a second unit to the **left neutral Goal**. It already has 1 half, so this makes 2 halves there (the 2nd Goal). Needed for the 3-Goal Worlds-qualifier version.
   - For elims: stack a third Pin on the Alliance Goal.
5. **End**: stop at least 2" off every wall and off the Toggle.
- **Total**: about **30-40 points**, 3-4 AWP halves and 1-2 qualifying Goals.
- **Risks**:
  - Over-rotating the Toggle leaves it unseated, so it counts as neutral.
  - Touching the Toggle or perimeter at 0:00 loses the AWP.
  - Pin orientation is wrong after grabbing.
  - Drifting toward the y = -x line. Keep x + y < -15 for the robot center.

### 3.3 Red 2 / bottom Quadrant: mirror of 3.2 (your `RightAuto` / `RightAwp`)
- Use the same route reflected across y = x:
  - Toggle at (0, -70).
  - Alliance Goal at (-23.6, -47.2).
  - Neutral Goal at (23.6, -47.2).
  - Diagonal unit at (-23.6, -23.6), shared with Red 1; agree with your partner who takes it. Alternatives are the wall unit (-23.6, -68.7) or the (-47.2, -47.2) unit.
- **Two-Robot standard AWP** [inferred]: each Robot reaches at least 3 halves with at least 2 on its Alliance Goal. Together that is 6 halves and 2 Goals.
- **Worlds-qualifier AWP**: one Robot must also get 2 halves on its neutral Goal, giving 7 halves and 3 Goals.

### 3.4 Solo AWP (one Robot, partner has no auto)
1. Toggle L.
2. Preload to the Red 1 Alliance Goal.
3. Unit to the Red 1 Alliance Goal (2 halves).
4. Unit to the left neutral Goal (2 halves with the preplaced one). That is 5 halves and 2 Goals.
5. Then either:
   - drive about 60" to the bottom Toggle and flip it, which Owns the preplaced yellow in the bottom neutral Goal (6th half), or
   - carry one more unit to the Red 2 Alliance Goal.
- **Estimate**: 13-16 s. **High risk** with a one-Pin claw.
- Only attempt this with a fast lift and motion chaining.
- If time is short, reorder so the bottom-Toggle flip happens on the way. One far-Toggle flip is worth more per second than another stack.

### 3.5 Elims / Bonus-max autos (no AWP needed; win the 12-point Bonus)
- **Double toggle + max stack**: flip your near Toggle, then stack 3-4 units on the closest Goal. A partner on the other red Quadrant does the same.
  - The two preplaced neutral yellows alone are worth 20 once both red Toggles are set [inferred].
- **Line rush**: take an Autonomous-Line cross cluster on your side, such as (-23.6, 23.6) from Red 1 or (23.6, -23.6) from Red 2. Score its red/yellow Pins yellow-up in your Quadrant Goals. This denies the blue/yellow Pins to the opponent.
  - Risk: SG8 contact. Any accidental contact with their side gives them the Bonus and AWP.
- **Midfield red pin**: put a red/blue Pin red-up on the tall Goal.
  - It counts 5 in auton [Q&A 3282]. It also starts building the stack whose yellows you can own at the end by winning the Midfield.
  - Avoid in quals; it is too risky.

### 3.6 Programming Skills (60 s, start Red 1, Goals empty, place anywhere, park at the end)
**Route skeleton** [inferred]:
1. **Left Toggle to red**, with the Preload yellow-up on the left neutral Goal (-47, 24): +10.
2. **Left-Quadrant units**:
   - wall units at (-68.7, ±23.6)
   - diagonal units at (-23.6, -23.6) and (-47.2, -47.2)
   - stack them on the Red 1 Alliance Goal and the left neutral Goal: about +40.
3. **Loader cycles at a red Loader** (-68.5, ±58.7). The drive team feeds nested Cup + Pin units (3 R/Y, 4 B/Y, 7 Cups). Each cycle is about 4-5 s to a Goal about 35" away: +10 each, yellow-up.
4. **Bottom Toggle to red**. Units at (-23.6, -68.7) and (23.6, -68.7) go to the Red 2 Alliance Goal and the bottom neutral Goal: about +30-40.
5. **Optional, for 150+**: cross to the blue Quadrants. Set the **top and right Toggles to blue** (Quadrant color). Fill those Goals with yellow-up units: +10 each.
6. **Last 3-4 s**: drive into the Midfield (|x|+|y| < about 20) and stop: **+8**. This also Owns any yellow you placed on the tall Goal.

**Benchmarks**:
- 98-108 is about 9-10 yellow halves plus park.
- 128 (a top-10 US programming score) is 12 halves plus park.
- 163 (the world-best) is about 15-16 halves plus park.
- Reliable 60-90 (6-8 halves plus park) would put 5776T well above the median.

**What makes skills routes reliable**:
- A wall reset after every Quadrant change.
- GPS: the GPS strip is installed in Autonomous Coding Skills.
- Fixed Loader pickup poses.
- Timeouts on every claw grab, using the claw distance sensor.

### 3.7 Fit with 5776T's mechanisms [inferred]
- **Toggle rollers + color sensor**: well suited to the toggle-first openings (3.2-3.5) and to setting all 4 Toggles in skills.
- **Pneumatic claw with auto-grab distance sensor + lift**: well suited to 1-unit-at-a-time stacking. The 1 Pin + 1 Cup limit means a claw is not a handicap.
- **Twist motor**: use it to force **yellow-up** on red/yellow Pins (10 points instead of 5).
- **Pin intake**: make sure it never holds a second Pin while the claw holds one (SG6).

---

## 4. Programming tips for an RW-Template (PROS, field-centered, distance resets, MCL) codebase

1. **One frame, generated mirrors.**
   - Author every route once in red / Red 1 field coordinates.
   - Generate Red 2 with (x, y) → (y, x), θ → 90° − θ.
   - Generate blue with (x, y) → (−x, −y), θ + 180°.
   - Flip `toggleAllianceColor` and the "which half up" choice with alliance color.
   - This replaces the current `setLocalOrigin` / `fx` / `fy` local-coordinate patching.
2. **Geofence the Autonomous Line.**
   - In auton, assert the robot footprint stays on your side: for red, x + y + about 12.7·√2 < 0 for the robot center, using a radius of about 12.7".
   - Whitelist only the deliberate cross-cluster grabs. An SG7 touch gives away both the Bonus and the AWP.
3. **Distance-sensor resets: know what blocks the walls** [inferred from layout].
   - **Toggle**: covers about |along-wall| < 13" at every wall center and sticks out a little.
   - **Wall stacks**: cover about 12-36" from center along every wall until they are taken.
   - **Loaders and Load Zones**: in the alliance-wall corners (|y| > about 50 on the x = ±70 walls).
   - **Clean windows**: roughly 36-50" from center along a wall, or a corner after its wall stack is gone.
   - **Gating**:
     - Gate every reset on |heading error| < 5-7°.
     - Gate on reading < about 40".
     - Gate on |measured − predicted| < about 4".
     - Use `distanceReset()` to log skipped resets.
4. **MCL.**
   - `mclCorrect` models only the square walls (`field_half_size` = 70.25), so drop any beam whose predicted ray hits a Toggle, a Goal (octagons at the coordinates in 1.3) or an unclaimed wall stack before the wall.
   - Or extend the raycast with those obstacles.
   - Mount sensors high enough to clear lying Pins and Cups (6.5" objects). Goal and Toggle heights still matter. **Measure them.**
5. **Toggle control loop.**
   - Run the optical LED at 100%. Use hue **and** saturation **and** proximity gates (your `toggle.cpp` already does).
   - Stop on the alliance face, brake-hold briefly, then **reverse off** so the Toggle settles seated and untouched (SC4).
   - Re-read after release. If the color is wrong or the Toggle is unseated, nudge again with a timeout.
   - The Toggle's starting orientation is shown in an Appendix A drawing (probably yellow; **unverified**).
6. **End-of-auton checklist** (the AWP needs all of these):
   - Not touching the perimeter.
   - Not touching the Toggle.
   - Not holding two Pins.
   - Last motion ends at least 2" off walls.
   - Log `secondsSinceAutonStart()` at every checkpoint. Keep about 0.7 s slack.
7. **Motion chaining.**
   - Use early-exit and min-speed on transit waypoints, and full settle only at Goal drops.
   - Run lift moves async during drives.
   - Low Alliance Goals (3.25") need precise final alignment. Consider a front distance sensor or AI Vision on the Goal **AprilTags** [Q&A 3294] for the last 6".
8. **Claw auto-grab.**
   - Median-filter the claw distance sensor and require N consecutive in-range reads.
   - Add a grab timeout so a missed unit doesn't stall the route.
   - Pins are about 1.6" in diameter and Cups about 3.15".
9. **Skills specifics.**
   - The GPS strip is installed for Autonomous Coding Skills. Fuse GPS with odometry, or use it for a one-shot re-localize after crossing quadrants.
   - Script Loader pickups at fixed poses so the human feeder can match the timing.
   - Finish with a Midfield park. Endgame rules don't apply in skills.
10. **Legacy code cleanup.** Rename Push Back-era symbols to Override terms so autonomous selection stays unambiguous:
    - `LONG_GOAL` → `ALLIANCE_GOAL`
    - `MIDDLE_GOAL` → `TALL_GOAL`
    - "matchloader" → Loader

---

## 5. Sources

**Official**
- Game Manual v2.1 (online): https://manuals.vex.com/v5/override/v2.1/game/ , https://manuals.vex.com/v5/override/v2.1/skills/ , https://manuals.vex.com/v5/override/v2.1/changelog/
- Rule pages: https://events.vex.com/storage/game_manual/VEX_V5_Robotics_Competition_2026-2027_Override/rules/SC1.html (also SC2, SC3, SC4, SC5, SC6, SC7, SC8, SG6, SG11, SG12, RSC3, RSC4, index.html in the same folder)
- Obsolete PDFs (figures, changelog, glossary):
  - https://content.vexrobotics.com/docs/2026-2027/override/files/override-v1.1-obs.pdf
  - https://content.vexrobotics.com/docs/2026-2027/override/files/v5rc-override-1.0-obsolete.pdf
- VEX VR Override playground (GPS coordinates): https://api.vex.com/vr/home/playgrounds/v5rc_override.html
- KB: https://kb.vex.com/hc/en-us/articles/51426592122004-Using-the-VEX-V5-26-27-Override-Playground-Window

**Q&A**
- Index: https://events.vex.com/V5RC/2026-2027/QA (pages 1-8)
- Rulings cited: /QA/3135, /3153, /3154, /3171, /3175, /3179, /3180, /3188, /3191, /3196, /3199, /3236, /3242, /3257, /3262, /3282, /3294
- Each is under https://events.vex.com/V5RC/2026-2027/QA/

**Skills data**
- https://events.vex.com/api/seasons/204/skills?post_season=0&grade_level=High%20School and the same with grade_level=Middle%20School (season 204 = Override)
- Standings page: https://events.vex.com/robot-competitions/vex-robotics-competition/standings/skills
- Event pages: https://events.vex.com/robot-competitions/vex-robotics-competition/RE-V5RC-26-4557.html , .../RE-V5RC-26-4846.html , .../RE-V5RC-26-5477.html , .../RE-V5RC-26-4246.html

**Third-party (lower confidence)**
- https://spartandesignrobotics.org/override-manual-summary , https://spartandesignrobotics.org/override-toggle-strategy , https://spartandesignrobotics.org/override-endgame
- https://www.atlantahobby.com/blogs/robotics/vex-v5-override-game-breakdown-scoring
- https://goodebot.net/override/
- YouTube titles only: https://www.youtube.com/watch?v=SjLbJZK9iZo , https://www.youtube.com/watch?v=NkrMeY1xYVk , https://www.youtube.com/watch?v=uut1jd7-kAI
- VEX Forum: the fetcher was blocked (403), so only search snippets were used. Threads seen:
  - https://www.vexforum.com/t/v5rc-override-game-manual-updates/146819
  - https://www.vexforum.com/t/auton-strategy-override/147107
  - https://www.vexforum.com/t/thoughts-on-any-strategies-for-override-alliance-matches-skills-auton/146003
  - https://www.vexforum.com/t/which-lift-is-best-for-override/146049
- GitHub:
  - https://github.com/cytsai11/vex-override-sim (layout model; consistent with the VR coordinates)
  - https://github.com/AnshuPlayz17/19109m-2026-2027-override (wall reset)
  - https://github.com/daydayparker/VexOverride569C (toggler)
  - https://github.com/Slimedguy/29295A-2026-2027-Vex-Override-code
  - https://github.com/nirvaankohli/vex-override

**Could not access or verify**
- r/vex (fetch blocked).
- VEX Forum post bodies.
- Signature-event award results.
- Exact Midfield tape and Load Zone dimensions.
- Toggle starting orientation and height.
- Cup and Goal diameters beyond the glossary.
- The exact v2.0 / v2.1 changelog text (taken from a 3rd-party summary).
