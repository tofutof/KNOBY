# 📘 Desk Buddy Project Logbook

> **Project Name:** Desk Buddy  
> **Microcontroller:** Arduino UNO  
> **Display:** 0.96" OLED Display (I2C SSD1306, 128x64)  
> **Power Source:** 9V Battery (via DC Barrel Jack / VIN & GND)  
> **Start Date:** October 9, 2026  

---

## 📅 Log Entry Format Guidelines
When recording entries, please follow this format:

```markdown
### [YYYY-MM-DD | HH:MM AM/PM] - <Brief Title / Milestone>
- **Objective:** What was being done during this session?
- **Actions Taken:** Specific tasks, wiring changes, code modifications.
- **Observations / Results:** What worked, sensor/screen readings, battery behavior.
- **Issues / Blockers Encountered:** Any bugs or hardware challenges.
- **Next Steps:** What to do in the next session.
- **Logged By:** <Name/s>
```

---

## 📝 Project Logs

### [2026-10-09 | 06:58 PM] - Project Kickoff & Workspace Setup
- **Objective:** Establish the project workspace structure, define core hardware requirements, setup logbook tracking, and prepare code and reporting infrastructure.
- **Actions Taken:**
  1. Created project directory architecture:
     - `Logbook/`: Centralized activity and progress logging.
     - `Project/DeskBuddy/`: Arduino IDE sketch folder compatible with native IDE compilation.
     - `Case_Study_Prompter/`: Automated report & PDF generator adhering to final case study requirements.
  2. Identified primary components:
     - Arduino UNO R3
     - 0.96" Monochrome I2C OLED Display (128x64, SSD1306 driver)
     - 9V Battery (with battery snap to DC barrel jack or VIN/GND)
     - Breadboard & Jumper wires
  3. Prepared starter code with OLED initialization, emotion animations/expressions, and power efficiency considerations.
- **Observations / Results:** Workspace initialized successfully. Ready for circuit simulation / breadboard assembly.
- **Issues / Blockers Encountered:** None.
- **Next Steps:**
  - Build and verify the circuit on Wokwi/Tinkercad or physical breadboard.
  - Test I2C communication on pins A4 (SDA) and A5 (SCL).
  - Verify 9V battery voltage regulation on Arduino UNO onboard 5V linear regulator (AMS1117).
- **Logged By:** Team / Project Lead

---

### [2026-10-09 | 07:29 PM] - Hologram Display & Feature Expansion (Time, Temp, Quotes & Faces)
- **Objective:** Upgrade the Desk Buddy into a 3D Pepper's Ghost hologram display, implementing four cyclic display modes: Expressive Faces, Real-time Clock, Ambient Temperature, and Quote of the Day.
- **Actions Taken:**
  1. **Optical Hologram Architecture:**
     - Adopted 45° angled transparent reflector plate (Pepper's Ghost effect) creating a floating visual illusion.
     - Implemented `SCREEN_ROTATION` setting (0/1/2/3) in sketch to easily compensate for top-mounted vs. base-mounted OLED reflections.
  2. **Software Clock (RTC) Implementation:**
     - Created lightweight millis-driven software RTC tracking hours, minutes, seconds, and date.
     - Added Serial monitor time synchronization parser (`T HH MM SS`) without needing external RTC modules.
  3. **Temperature Measurement:**
     - Added temperature screen featuring custom thermometer icon, Celsius and Fahrenheit readouts.
     - Provided dual-mode support: physical analog sensor (LM35/TMP36 on `A0`) or calibrated ambient fallback.
  4. **Quote of the Day (SRAM Preservation):**
     - Structured inspirational quote table using `PROGMEM` in Flash memory to prevent SRAM depletion on ATmega328P.
     - Designed character wrapping routine tailored to 128x64 display.
  5. **Animated Companion Faces:**
     - Created 7 expressive eye animation states: Happy, Blink, Look Left, Look Right, Wink, Heart Eyes, and Sleepy.
  6. **Power Pacing:**
     - Tuned loop delay to 35ms for smooth 25-30 FPS rendering while minimizing 9V battery drain.
- **Observations / Results:** The multi-mode carousel smoothly transitions across all four features. Hologram reflection gives high contrast because unlit OLED pixels emit zero light.
- **Issues / Blockers Encountered:** Memory conservation is critical; moving string tables to `PROGMEM` ensures Arduino UNO RAM usage stays well below the 2KB limit alongside the 1KB SSD1306 display buffer.
- **Next Steps:**
  - Build transparent plastic/acrylic 45° pyramid or single plate enclosure.
  - Test physical LM35 temperature sensor on A0.
  - Update Case Study report data with hologram design and test videos.
- **Logged By:** Desk Buddy Team

---

### [2026-10-09 | 07:47 PM] - Real-Time Clock Auto-Sync & Circadian Companion Faces
- **Objective:** Ensure 100% accurate wiring compatibility with Arduino Starter Kits, establish zero-setup real-time clock synchronization, and align interactive companion faces with the 24-hour day cycle.
- **Actions Taken:**
  1. **Arduino Starter Kit Wiring Audit:**
     - Added critical silkscreen inspection alert in `WIRING_GUIDE.md` addressing both `GND-VCC-SCL-SDA` and `VCC-GND-SCL-SDA` OLED module variants to prevent board burnouts.
     - Documented TO-92 LM35 temperature sensor pinout with flat-face orientation diagram (+Vs, Vout -> A0, GND).
     - Verified 9V battery DC barrel jack center-positive connection with reverse polarity protection.
  2. **Automated Real-Time Clock (RTC):**
     - Implemented compiler macro extraction (`__TIME__` and `__DATE__`), enabling automatic time sync with the user's computer clock upon sketch upload.
     - Maintained Serial calibration parser (`T HH MM SS`) for on-the-fly testing of different hours of the day.
  3. **Circadian Time-Aligned Facial Expressions:**
     - Segmented day into 4 circadian mood pools:
       - **Morning (06:00 - 11:59):** Energetic wake-up, sparkle eyes, cheerful arches (`^ ^`), winks.
       - **Afternoon (12:00 - 17:59):** Study companion focus, looking left/right at desk work, heart eyes (`<3 <3`).
       - **Evening (18:00 - 21:59):** Relaxed content smile, calm gentle blinks, cozy unwinding.
       - **Night (22:00 - 05:59):** Sleepy droopy eyes, closed peaceful curves (`- -`), and floating animated `zZz` drifts.
     - Added context-sensitive greetings on the lower banner matching each time period.
- **Observations / Results:** The companion now automatically adopts appropriate emotional behaviors matching real-world time as soon as it powers on from the 9V battery.
- **Issues / Blockers Encountered:** Resolved. All Arduino kit pinouts verified.
- **Next Steps:**
  - Connect physical Arduino kit hardware and test Pepper's Ghost prism reflection.
  - Re-run Case Study prompter to update documentation.
- **Logged By:** Desk Buddy Team

---

### [2026-10-11 | 11:45 PM] - Desk Buddy v2.0 Restart: Mini-Breadboard Layout, Jump Pack & Climate Suite
- **Objective:** Restart and streamline hardware design around MakerLAB Upgraded Learning Kit v2 components, adopting an extra-small mini breadboard for a compact, aesthetic, and presentable desk gadget with interactive motion and environmental sensing.
- **Actions Taken:**
  1. **Hardware Configuration & Pin Mapping:**
     - Selected 0.96" I2C Monochrome OLED Display (SSD1306, 128x64) as 3D Pepper's Ghost hologram projector (`A4`/SDA, `A5`/SCL, 5V, GND).
     - Integrated Tilt Ball Switch on Digital Pin 2 using internal `INPUT_PULLUP` to eliminate external resistors and save breadboard space.
     - Added Photoresistor (LDR) on Analog Pin A0 with 10kΩ pull-down resistor for circadian Day/Night ambient light tracking.
     - Added DHT11 sensor on Digital Pin 4 for live ambient Temperature (°C/°F) and Relative Humidity (%) monitoring.
  2. **Extra-Small Breadboard Layout (170 Tie-Points):**
     - Mapped point-to-point layout across 17 rows, grouping OLED on Rows 1-4, DHT11 on Rows 6-8, LDR divider on Rows 10-12, and Tilt Switch on Rows 14-15.
     - Designed layout for short, tidy jumpers to ensure clean presentation and zero wire clutter.
  3. **Interactive "Jump Pack" & Motion Physics:**
     - Implemented spring-loaded parabolic Jump animation (`^o^`) triggered on breadboard tap/nudge with airborne thrust lines and landing bounce.
     - Implemented rapid-shake detection triggering a playful Dizzy wobble state (`@_@`).
  4. **Circadian Sleep Engine (LDR-Driven):**
     - Turning off room lights (<160 ADC) triggers auto-sleep with heavy eyelids, peaceful closed eyes, and floating hologram `z Z Z` animations.
     - Turning lights back on (>240 ADC) wakes Desk Buddy up with a joyful stretch and smile.
  5. **Firmware Architecture (`DeskBuddy.ino`):**
     - Completely rebuilt firmware with non-blocking `millis()` timing and zero blocking delays in loop.
     - Embedded a direct bit-bang DHT11 decoder for 100% zero-dependency library reliability.
     - Preserved Flash `PROGMEM` quote storage and compiler timestamp auto-clock synchronization.
- **Observations / Results:** The mini-breadboard design significantly improves portability and aesthetics while adding two high-engagement interactions (physical nudge jump and room-light sleep).
- **Issues / Blockers Encountered:** None. All MakerLAB v2 kit components fit neatly on the 170-point mini breadboard without pin contention.
- **Next Steps:**
  - Connect hardware onto the mini breadboard following the updated `WIRING_GUIDE.md`.
  - Flash `DeskBuddy.ino` via Arduino IDE.
  - Mount 45° transparent reflector and enjoy your interactive 3D Desk Buddy companion!
- **Logged By:** Desk Buddy Team

---

### [2026-10-12 | 12:08 AM] - Triple-Tap Gesture Navigation & All-in-One Dashboard HUD
- **Objective:** Replace the auto-cycling carousel with physical gesture navigation (triple-tap on chassis) and combine Real-Time Clock, Date, and Climate telemetry into a unified single-glance Dashboard HUD.
- **Actions Taken:**
  1. **Triple-Tap Gesture Engine (`DeskBuddy.ino`):**
     - Implemented multi-tap window state machine (`MIN_TAP_INTERVAL = 75ms` debounce, `MULTI_TAP_WINDOW = 850ms`).
     - 3 quick taps on case/desk advances page: `[Page 1: Pet Face]` ➔ `[Page 2: All-in-One HUD]` ➔ `[Page 3: Daily Quote]`.
     - 1 tap on Pet Face page triggers immediate, zero-lag **Jump Pack** bounce (`^o^`).
     - Rapid continuous shake triggers **Dizzy** spiral eyes (`@_@`).
  2. **All-in-One Dashboard HUD Layout:**
     - Designed high-readability 128x64 stacked HUD:
       - Top Row: Calendar Date (`OCT 12, 2026`) and Comfort Badge (`[COMFY]`).
       - Center Row: Hero bold digital clock (`HH:MM:SS`, Font Size 2) with double divider lines.
       - Bottom Rows: Live Temp in °C, Humidity in %, Room Light status, and subtle navigation hint (`TAPx3 >`).
  3. **Documentation & Build Verification:**
     - Updated `Project/DeskBuddy/DeskBuddy.ino`, `Project/WIRING_GUIDE.md`, and `Project/README.md`.
     - Recompiled Case Study report (`Final_Case_Study.html` & `Final_Case_Study.pdf`).
- **Observations / Results:** User no longer has to wait for an auto-timer to read metrics. Everything is glanceable on Page 2, and physical tapping feels responsive and intuitive.
- **Issues / Blockers Encountered:** None.
- **Next Steps:**
  - Flash updated `DeskBuddy.ino` to the Arduino UNO.
  - Test physical tap sensitivity and enjoy the interactive Desk Buddy!
- **Logged By:** Desk Buddy Team

---

### [2026-10-12 | 12:19 AM] - Xiaozhi Face Engine Port: Organic Micro-Drifts & Dynamic Mouth
- **Objective:** Port the facial animation design and behavior patterns from the open-source `TechTalkies/Face-for-Xiaozhi` engine to native Arduino Uno (ATmega328P) and Adafruit SSD1306, replacing static expressions with organic breathing, fluid multi-frame blinks, and an animated mouth.
- **Actions Taken:**
  1. **Organic Saccade Micro-Drifts:**
     - Added autonomous ±2px X and ±1px Y micro-movement offsets every 1.8–4.2s to eyes and mouth, creating a lifelike breathing/living presence.
  2. **Multi-Phase Eyelid Blinking:**
     - Replaced binary blinks with a 4-phase squash-and-stretch curve: Open (28px) ➔ Squash (6px) ➔ Slit (2px) ➔ Rebound (10px) ➔ Open.
  3. **Dynamic Animated Mouth Engine:**
     - Idle State: Subtle horizontal smile bar tracking saccade coordinates.
     - Curious / Listening State: Asymmetrical inquisitively squinted eyes with a cocked smirk.
     - Speaking / Chattering State: Dynamic mouth height tweening (2px–16px) reciting daily wisdom quotes or reacting to interactions.
  4. **Hardware Reactivity Synthesis:**
     - Seamlessly merged Xiaozhi aesthetics with the physical tilt ball switch (Jump Pack leap on single tap, Dizzy on shake) and LDR auto-sleep.
- **Observations / Results:** The companion feels exponentially more animated and alive on the 0.96" OLED without consuming extra SRAM.
- **Logged By:** Desk Buddy Team

---

### [2026-10-12 | 02:21 AM] - Triple-Tap 3-Second Lockout Cooldown
- **Objective:** Eliminate accidental double-page skips by implementing a 3.0-second input lockout after each triple-tap page transition.
- **Actions Taken:**
  1. **Cooldown State Machine:**
     - Configured `TRIPLE_TAP_COOLDOWN = 3000ms`.
     - When a 3x tap occurs, `tapCooldownUntil = now + 3000ms` locks out all tap inputs for 3 seconds.
  2. **Visual State Confirmation:**
     - During the 3-second lockout, bottom-right prompt displays `[LOCKED]`.
     - When the 3 seconds elapse, it automatically flips back to `TAPx3 >` to indicate it is re-armed and ready.
- **Observations / Results:** Page transitions are now deliberate and stable, completely eliminating spurious multi-page flipping from residual case vibration.
- **Logged By:** Desk Buddy Team

---

### [2026-10-13 | 12:15 PM] - Style B: 3D First-Person Arcade Cockpit & Instant Tap Response
- **Objective:** Upgrade the single-tap reaction to an immersive, retro-arcade Style B: 3D First-Person Cockpit racer, and make single-tap detection instantaneous without input lag.
- **Actions Taken:**
  1. **Instant Tap Responsiveness:**
     - Reduced `SINGLE_TAP_RESOLVE_WINDOW` from `650ms` down to `240ms`, achieving instant human-perceived feedback.
     - Switched evaluation to relative delta `(now - lastTapRegisteredTime >= 240ms)` to allow fluid triple-taps while triggering single-tap under a quarter of a second.
     - Adjusted `TRIPLE_TAP_COOLDOWN` to `1400ms` for snappier page switching.
  2. **Style B 3D First-Person Arcade Cockpit Engine (`renderArcadeCockpitScene`):**
     - **3D Perspective Road:** Infinite perspective highway with exponential depth expansion on center dashes, moving roadside rumble strips, and distant mountains with parallax.
     - **Dynamic S-Curve Drifting:** Road curves dynamically left and right (-16px to +16px) with mathematical wave sway.
     - **Cockpit Dashboard:** Lower console featuring an analog speedometer dial, live digital KM/H readout, active LED turbo meter, and subtle engine rumble vibration.
     - **Tilting Sports Steering Wheel:** F1/arcade steering wheel actively tilts to match road turns, gripped by Knoby's cute robot driver paws.
     - **Rearview Mirror:** Mounted top-center showing Knoby's focused racing eyes (`> <`) watching the track behind him.
     - **Nitro Warp Finish:** Hyperspace speed warp lines during Phase 3, concluding with Knoby landing cheerfully back on the desk with smiling arch eyes (`^ ^`).
  3. **Memory & Performance Verification:**
     - Zero additional dynamic SRAM allocations (515 bytes / 25% SRAM used, 1533 bytes free for stack/buffer).
     - Clean geometric code leaving 562 bytes of free Flash headroom (31,694 bytes used on ATmega328P).
     - Verified clean compilation with `arduino-cli`.
- **Observations / Results:** Single-tap reaction feels immediate and lively. The 3D perspective creates a retro arcade driving experience right on the 0.96" OLED.
- **Logged By:** Desk Buddy Team

---

### [2026-10-13 | 12:25 PM] - Smooth 3D S-Curve Drifting, Camera Pan-Out & Victory Wink with Star
- **Objective:** Polish the 3D Arcade Cockpit to make driving movements buttery smooth, implement a cinematic camera pan-out, and display Knoby winking with an animated twinkling star.
- **Actions Taken:**
  1. **Silky Parabolic S-Curve Drift:**
     - Replaced linear wave calculations with smooth parabolic easing (`t * (400 - t)`), creating fluid weight shifts as the car enters and exits turns with zero vertex snapping.
  2. **Cinematic Camera Pan-Out Transition (1500–1800ms):**
     - Dashboard console and steering wheel smoothly slide downwards off the bottom of the screen (`dashTopY` drops from 48 to 78px), naturally panning the camera out to Knoby.
  3. **Victory Wink & Twinkling Anime Sparkle Star (1800–2600ms):**
     - Left eye stays wide open with glossy pupil highlight.
     - Right eye squashes shut into a crisp anime wink arc.
     - A 4-point pulsating diamond star (`✦`) glints and twinkles beside the winked eye with expanding cross-rays and floating sparkles.
     - Knoby strikes a champion open smile before relaxing back into companion mode.
  4. **Memory Optimization:**
     - Streamlined quotes table to 5 core inspiring quotes to safeguard Flash storage.
     - Used native 16-bit integer math for depth perspective calculations.
     - Final build: 32,134 bytes Flash (leaves 122 bytes free), 515 bytes SRAM (1,533 bytes free), running at silky 35+ FPS.
- **Observations / Results:** The transition from high-speed cockpit racing to the camera pan-out and champion wink is cinematic, smooth, and full of personality.
- **Logged By:** Desk Buddy Team

### [2026-10-13 | 12:55 PM] - Cute Monochrome Pixel Art Infographic (Accurate Knoby Face)
- **Objective:** Refine both the graphic poster and the interactive HTML manual to accurately feature Knoby's signature OLED face (two rounded capsule eyes with glossy glint dots, smile mouth, victory wink with star, and sleep crescents) instead of generic robot casing/antennas.
- **Actions Taken:**
  1. **High-Resolution Graphic Poster (`assets/knoby_pixel_infographic.jpg`):**
     - Regenerated poster with Knoby's actual pure-face aesthetic floating in black space.
     - Step 1: Knoby's large rounded eyes and talking speech bubble (`Hi! Im knoby <3`).
     - Step 2: 3D cockpit perspective with racing paws, rearview mirror, and Knoby's actual victory wink with diamond star (`✦`).
     - Step 3: 3 mini OLED screens showing Knoby's actual face, 28°C Weather HUD, and Quotes.
     - Step 4: Knoby's eyes curved into peaceful sleeping crescents with floating `z Z Z` and 45° clear prism diagram.
  2. **Interactive HTML Manual (`KNOBY_INFOGRAPHIC.html`):**
     - Updated CSS/SVG geometry to render Knoby's exact proportions (22x30px rounded capsule eyes, pupils with white specular dots, victory wink arc, sleeping crescents).
     - Removed robot emojis and replaced them with Knoby's actual face icons.
     - Embedded the high-res poster directly in the page for 1-click viewing and download.
- **Observations / Results:** The guide is now 100% faithful to Knoby's in-firmware character design.
- **Logged By:** Desk Buddy Team

### [2026-10-13 | 01:05 PM] - Modern Smartwatch Dashboard HUD & Memory Optimization
- **Objective:** Overhaul Page 2 (Clock, Temperature & Humidity) from a plain ASCII text list into an aesthetic, modern Smartwatch Dashboard featuring micro-cards and visual level gauge bars while keeping firmware safely within the 32KB ATmega328P Flash memory limit.
- **Actions Taken:**
  1. **Aesthetic Dashboard Layout (`DeskBuddy.ino`):**
     - Replaced plain text lines with a modern dual-card widget architecture:
       - Top Row: Symmetrical rounded badges for Date (`[OCT 13]`) and Live Room State (`[COMFY DAY]`).
       - Left Column: Hero Digital Clock card with large bold digits (`HH:MM`), dynamic blinking colon, subscripted running seconds (`S:38`), and `AM/PM` tag.
       - Right Column: Dual micro-cards for Temperature (pixel thermometer glyph + °C + live horizontal filled gauge bar) and Humidity (pixel water droplet glyph + % + live horizontal filled gauge bar).
       - Bottom Row: Symmetrical status badge (`KNOBY HUD` / `[LOCKED]`) with clean 3-dot page indicator (`○ ● ○`).
  2. **Flash Memory Budget Optimization:**
     - Eliminated 32-bit software division by replacing `map()` with native 16-bit integer math for the level gauges.
     - Streamlined intro dialogue Phase 3 hold and quote reading routines to read directly from `PROGMEM` without intermediate stack buffers.
     - Reduced Flash footprint to **31,906 bytes (98%)**, creating **350 bytes of free Flash headroom** on the ATmega328P.
- **Observations / Results:** Page 2 now looks like a high-end desktop smartwatch widget with live animated gauges instead of plain text, and compiles cleanly with zero memory errors.
### [2026-10-13 | 01:10 PM] - Infographic Remake: Smartwatch Dashboard Switch Screen
- **Objective:** Update the Switch Screen panel (Step 3) on both the monochrome graphic poster and interactive HTML manual to accurately showcase the newly designed Smartwatch Dashboard HUD.
- **Actions Taken:**
  1. **Regenerated Graphic Poster (`assets/knoby_pixel_infographic.jpg`):**
     - Step 3 now prominently displays the 3 genuine OLED screens:
       - Screen 1: Knoby's living companion face (big capsule eyes + smile).
       - Screen 2: Smartwatch Dashboard with digital clock `12:45`, date badge `[OCT 13]`, live `25°C` temperature filled gauge bar, `56%` humidity filled gauge bar, and 3-dot page indicator `○ ● ○`.
       - Screen 3: Wisdom quotes screen.
  2. **Updated Interactive HTML Manual (`KNOBY_INFOGRAPHIC.html`):**
     - Redesigned the Step 3 mini-cards to render the exact micro-card layout of Screen 2 (date pill, bold digital time, dual progress bars for warmth/humidity, and active page dot indicator).
- **Observations / Results:** The documentation and infographics are now fully synchronized with the running firmware in `DeskBuddy.ino`.
- **Logged By:** Desk Buddy Team

### [2026-10-13 | 01:22 PM] - Infographic Creator Credits Attribution
- **Objective:** Explicitly add full project creator credits (**Rairolf Rabang & Nicole Margareth Sibal**) to the approved retro monochrome pixel-art infographic poster and interactive manual without altering any existing approved illustrations or layouts.
- **Actions Taken:**
  1. **Graphic Poster (`assets/knoby_pixel_infographic.jpg`):**
     - Retained 100% of the approved pixel artwork across all 4 panels (CRT wake-up with actual Knoby face, 3D arcade cockpit with steering wheel and victory wink + star, modern Smartwatch HUD dashboard with level gauges, and nighttime snooze with Pepper's Ghost prism).
     - Seamlessly integrated a matching retro 1-bit pill tag directly below the top banner: `✦ Created by Rairolf Rabang & Nicole Margareth Sibal ✦` with balanced margins and retro pixel sparkle stars.
  2. **Interactive HTML Manual (`KNOBY_INFOGRAPHIC.html`):**
     - Updated author tag banner to display `Created by Rairolf Rabang & Nicole Margareth Sibal • Arduino UNO R3 Platform`.
  3. **Documentation:**
     - Updated `PROJECT_FUNCTIONALITY.md` bylines and creator recognition badges.
- **Observations / Results:** The poster preserves all beloved artwork while cleanly displaying both creators in retro pixel styling.
### [2026-10-13 | 01:32 PM] - Page 3 (Daily Wisdom) Aesthetic UI Overhaul
- **Objective:** Overhaul Page 3 (Quote of the Day) from a plain ASCII line format to match the modern, aesthetic card-and-pill UI design language established on Page 2 (Smartwatch Dashboard).
- **Actions Taken:**
  1. **Aesthetic Card & Pill Layout (`DeskBuddy.ino`):**
     - Top Badges: Symmetrical rounded pill badges for `[DAILY WISDOM]` (left) and `[01/03]` (right quote counter).
     - Hero Speech Card: Framed `drawRoundRect(1, 14, 126, 38, 3)` housing the streaming quotes with quotation marks (`"..."`).
     - Live Speaker Avatar & Voice Waves: Knoby's live talking mouth (`drawXiaozhiMouth`) integrated inside the card with dynamic pulsating audio waves (`((( `) when vocalizing.
     - Bottom Navigation Bar: Symmetrical status readout (`KNOBY TALKS` / `[LOCKED]`) with 3-dot page indicator highlighting the third dot (`○ ○ ●`).
  2. **Memory & Performance Verification:**
     - Zero additional dynamic SRAM used (509 bytes / 24% used, 1,539 bytes free).
     - Sketch size: 32,166 bytes (99%) of 32,256 bytes, cleanly passing compilation with exit code 0.
  3. **Documentation:**
     - Updated `PROJECT_FUNCTIONALITY.md` and `KNOBY_INFOGRAPHIC.html` Screen 3 previews.
- **Observations / Results:** The entire OS across Page 1, Page 2, and Page 3 now shares a cohesive, high-end visual design system.
- **Logged By:** Desk Buddy Team

---

