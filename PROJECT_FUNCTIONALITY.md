# KNOBY v2.0: Interactive 3D Hologram Desk Companion

**Authors & Creators:** Rairolf Rabang & Nicole Margareth Sibal  
**Platform:** Arduino UNO R3 (Microchip ATmega328P @ 16 MHz)  
**Display:** 0.96" Monochrome I2C OLED (SSD1306, 128x64) with 45° Pepper's Ghost Optical Prism  
**Sensor Suite:** SW-520D Tilt Ball Switch, Analog Photoresistor (LDR), DHT11 Climate Sensor  
**Document Type:** Project Functionality Specification & User Guide  

---

## 1. Short Description of Project Functionality (Simple / Kid-Friendly)

> **"What is Knoby?"**  
> Imagine having a tiny, glowing cartoon robot pet that lives on your desk and floats right in the air like a hologram! That's **Knoby**!

* **He Feels Alive:** When you look at him, Knoby blinks his big shiny eyes, looks left and right around your room, and smiles warmly.
* **Knock to Race (3D Arcade Fun):** If you tap or knock on your desk, Knoby immediately hops into a 3D race car cockpit! You see the highway rushing toward you, mountains in the background, a steering wheel that tilts as he drifts around curves, and Knoby's cute little paws driving. Then he hits turbo **NITRO**, the camera zooms out, and Knoby gives you a victory wink with a twinkling sparkle star!
* **A Real Desk Helper:** Tap your desk three times quickly, and Knoby switches to a smart clock that shows the exact time, the room temperature, and the humidity so you know if your room is cozy. Tap three times again, and he reads you an inspiring quote to brighten your day.
* **Sleeps When You Sleep:** When you turn off your bedroom light at night, Knoby automatically gets sleepy, closes his eyes, and snores with floating `z Z Z` holograms. When morning comes and you turn the light back on, he cheerfully wakes up with a big smile!

---

## 2. Technical Executive Summary

**KNOBY v2.0** is an interactive, micro-optics desktop robotics companion powered by a single 8-bit Microchip ATmega328P microcontroller. Utilizing the classic **Pepper's Ghost** optical beam-reflection illusion, Knoby projects crisp, high-contrast imagery from a 0.96" I2C OLED display onto a 45-degree semi-reflective prism, making graphics appear suspended in three-dimensional space with no bulky headset required.

Knoby combines autonomous artificial life behaviors (stochastic saccadic eye movements, pupil depth parallax, multi-frame squash-and-stretch blinking) with physical desktop gesture recognition, real-time climate monitoring, circadian sleep cycles, and a custom integer-based 3D perspective racing engine running at over **35 frames per second**.

---

## 3. Detailed System Functionality

### 3.1 Power-On & Introduction Sequence
Upon receiving 5V power via USB or battery, Knoby executes a sequential three-phase startup routine:

1. **Creator Recognition Badge:** Displays the official project title (**KNOBY v2.0**) alongside creators **Rairolf Rabang & Nicole Margareth Sibal**.
2. **Retro Cathode-Ray Tube (CRT) Television Bloom:** Simulates vintage phosphor ignition. A sharp center dot expands into a high-intensity horizontal laser line, blooms vertically across the display, flashes with a simulated phosphor surge, and dissolves cleanly into black.
3. **Awakening & Synchronized Typewriter Dialogue:**
   * Knoby curls his eyelids open into happy smiling arcs (`^ ^`).
   * A cartoon dialogue bubble renders with a letter-by-letter typewriter animation: **"Hi! Im knoby <3"**.
   * Knoby's mouth chatters up and down in synchronization with each incoming character, mimicking spoken dialogue.

---

### 3.2 Page 1: Autonomous Companion & Interactive 3D Driving Cutscene

#### A. Autonomous Living Idle State
While sitting idle, Knoby demonstrates organic pet behaviors:
* **Natural Gaze Wandering (Saccades):** Eyes shift across five distinct focal directions (glancing left -13px, glancing right +13px, looking upward inquisitively -7px, resting downward, or centered), dwelling organically for 1.5 to 3.5 seconds before smoothly returning.
* **3D Depth Parallax:** Inner dark pupils and white specular glint dots shift within the eye sockets with dampened parallax displacement, creating the visual impression of a spherical eyeball.
* **Smooth Squash-and-Stretch Blinking:** Eyelids close and open through a four-frame squash trajectory every 2.5 to 6.0 seconds.
* **Head-Turn Tracking:** The mouth shifts dynamically along the X-axis following eye direction, simulating the physical rotation of a rounded head.

#### B. Single-Tap Desk Gesture: 3D First-Person Arcade Cockpit & Victory Wink
Knocking or tapping the desk triggers an instant physical reaction via the hardware-interrupt vibration sensor:
* **Ultra-Responsive Debounce (<240ms):** Tap gestures resolve within a swift 240 ms time window, eliminating input latency.
* **Cinematic 5-Phase Cutscene (2.6-second total duration):**
  1. **Launch (0 – 350 ms):** The screen transforms into the driver's perspective inside a sports race car. The dashboard vibrates with subtle engine rumble, and the digital speedometer accelerates rapidly from 0 to 160 KM/H.
  2. **Silky Parabolic S-Curve Drifting (350 – 1150 ms):** The 3D perspective highway sweeps dynamically left and right along a continuous parabolic curve. An F1-style sports steering wheel tilts smoothly with the turns, held by Knoby's cartoon driver paws. Background mountains shift with realistic parallax, road dashes expand exponentially toward the windshield, and a top-center rearview mirror reveals Knoby's racing eyes (`> <`).
  3. **Nitro Warp Speed (1150 – 1500 ms):** The HUD flashes `"NITRO!"` as hyperspace speed lines radiate from the vanishing point.
  4. **Smooth Camera Pan-Out (1500 – 1800 ms):** As the car crosses the finish line, the cockpit dashboard and steering wheel smoothly slide downward off-screen, pulling the viewer's camera back to Knoby.
  5. **Victory Wink & Twinkling Sparkle Star (1800 – 2600 ms):** Knoby strikes a victory pose: his left eye stays open with specular gloss, his right eye curves into an anime victory wink (`⌒`), and an animated 4-point diamond star (`✦`) twinkles beside his winked eye with expanding radiant cross-rays!

---

### 3.3 Page 2: Modern Smartwatch Dashboard HUD (Clock, Climate & Gauges)
Tapping the desk **three times rapidly** (*tap-tap-tap*) navigates to the redesigned aesthetic Smartwatch Dashboard:
* **Top Status Bar Pills:** Symmetrical rounded badges displaying calendar date (`[OCT 13]`) on the left, and live room status (`[COMFY DAY]` / `[HUMID NTE]`) on the right.
* **Hero Clock Widget (Left Card):** A bold, rounded card housing the digital clock with large primary digits (`HH:MM`), a smooth pulsing colon, subscripted running seconds (`S:38`), and period indicator (`AM/PM`).
* **Interactive Temperature Widget (Top-Right Card):** A dedicated micro-card featuring a pixel thermometer glyph, current room temperature in °C, and an active **horizontal filled gauge bar** that visually indicates comfort temperature.
* **Interactive Humidity Widget (Bottom-Right Card):** A dedicated micro-card featuring a pixel water droplet glyph, relative humidity percentage (%), and an active **horizontal filled gauge bar** that tracks moisture levels.
* **Minimalist Navigation Footer:** Replaces cluttered text with a sleek brand badge (`KNOBY HUD`) and a symmetrical 3-dot page indicator (`○ ● ○`) highlighting active Page 2.

---

### 3.4 Page 3: Daily Inspiration & Wisdom Quotes (Smartwatch Speech Card)
A subsequent **triple-tap gesture** transitions to Knoby's motivational library, sharing the aesthetic card-and-pill design language of Page 2:
* **Top Header Pills:** Symmetrical rounded badges displaying `[DAILY WISDOM]` and current quote progress `[01/03]`.
* **Hero Rounded Speech Card:** A high-contrast rounded card (`drawRoundRect`) framing the streaming quote with opening and closing quotation marks (`"..."`).
* **Vocalizing Avatar & Audio Waves:** Knoby signs off with `- Knoby`, his live chattering mouth animating rhythmically, flanked by dynamic pulsing audio soundwaves (`((( `) simulating spoken speech into the room.
* **Synchronized Bottom Bar & 3-Dot HUD:** Status readout (`KNOBY TALKS` / `[LOCKED]`) with a 3-dot page indicator highlighting the third dot active (`○ ○ ●`).

---

### 3.5 Circadian Auto-Sleep & Morning Awakening
An analog photoresistor (LDR) continuously evaluates ambient workspace illumination:
* **Bedtime Sleep (<160 ADC):** When room lights are extinguished, Knoby's eyelids droop into sleeping crescents, and he enters a low-activity sleep state with animated `z Z Z` letters floating upward.
* **Temporary Wake on Tap:** Tapping the desk while Knoby is asleep wakes him for 12 seconds with sleepy, gentle eyes before he drifts back to sleep.
* **Morning Awakening (>300 ADC):** When room lighting is restored, Knoby opens his eyes and bounces happily back to life.

---

## 4. Hardware Architecture & Pin Mapping

| Component | Part Model | Pin / Bus Connection | Operating Voltage | Function |
| :--- | :--- | :--- | :--- | :--- |
| **Microcontroller** | Arduino UNO R3 (ATmega328P) | 16 MHz Clock | 5V DC (USB / Barrel) | Executes animation state machine, math calculations, and sensor polling. |
| **Monochrome OLED** | 0.96" SSD1306 (128x64) | I2C: SDA -> A4, SCL -> A5 | 5V DC | Projects high-contrast graphics into the optical prism. |
| **Vibration Sensor** | SW-520D Tilt Ball Switch | Digital Pin 2 (External INT0) | 5V (Internal Pull-Up) | Detects desk taps and knocks with zero standby power draw. |
| **Light Sensor** | Mini Photoresistor (LDR) | Analog Pin A0 | 5V (10kΩ Divider) | Senses room light levels for automated sleep/wake transitions. |
| **Climate Sensor** | DHT11 Digital Sensor | Digital Pin 4 | 5V DC | Measures ambient temperature and relative humidity. |
| **Optical Prism** | 45° Clear Acrylic / PET Reflector | Passive Mechanical | N/A | Reflects OLED display upwards to produce the Pepper's Ghost 3D hologram. |

---

## 5. Software Engineering & Microcontroller Optimization

1. **400 kHz Fast-Mode I2C Bus:** Configured using `Wire.setClock(400000)`, reducing frame transfer time from ~100 ms down to ~24 ms, achieving a fluid **35+ frames per second (FPS)** refresh rate without screen flicker.
2. **Hardware Register Optical Inversion:** Sends direct SSD1306 command `0xA0` (Segment Remap) during initialization. This mirrors the X-axis in display hardware, ensuring graphics and text reflect correctly in the 45° glass without consuming CPU cycles or memory.
3. **Fixed-Point & Integer 3D Geometry:** All 3D perspective lines, parabolic road curves, and scaling formulas utilize 16-bit integer mathematics, avoiding slow floating-point operations on the 8-bit AVR architecture.
4. **Zero Dynamic Allocation (Safe SRAM Footprint):** Uses zero dynamic memory allocation (`malloc`, `free`, or Arduino `String` objects). Consumes only **515 bytes** of SRAM (25%), leaving ample stack space and safeguarding the 1024-byte OLED display buffer from memory corruption.
5. **Flash Memory Budgeting:** The entire animation engine, 3D math routines, sensor drivers, and text assets fit within **32,134 bytes** (99.6% of available ATmega328P Flash), maintaining stable, long-term operational integrity.

---

## 6. User Operating Guide

* **Power On:** Connect the Arduino UNO to any 5V USB port or 9V DC adapter. Enjoy the creator badge, CRT power-on bloom, and Knoby's talking intro.
* **Play / Race (Single Tap):** Knock or tap firmly on the desk surface next to Knoby. Watch him launch into 3D first-person racing, drift around corners with F1 steering, boost with nitro, and celebrate with a victory wink and twinkle star.
* **Switch Screens (Triple Tap):** Knock three times in rapid succession (*tap-tap-tap*) to cycle through:
  * **Screen 1:** Living Pet Face & 3D Racing
  * **Screen 2:** Live Clock, Temperature & Humidity HUD
  * **Screen 3:** Daily Motivational Wisdom Quotes
* **Nighttime Sleep:** Turn off your room lights. Knoby will automatically close his eyes and drift off to sleep with floating `z Z Z` holograms.
