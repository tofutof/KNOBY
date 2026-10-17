# 🔌 Complete Hardware & Wiring Guide: 3D Hologram Desk Buddy v2.0

> **Platform:** Arduino UNO R3 (MakerLAB Upgraded Learning Kit v2)  
> **Display:** 0.96" I2C Monochrome OLED (SSD1306, 128x64)  
> **Prototyping:** Extra-Small Mini Breadboard (170 Tie-Points)  
> **Interactive Modules:** Tilt Ball Switch ("Jump Pack"), Photoresistor (LDR), DHT11 Temp/Humidity Module  
> **Optics:** 45° Pepper's Ghost Clear Acrylic/Plastic Reflector  
> **Power:** 9V Battery Snap (DC Jack) or USB 5V  

---

## ⚠️ STEP 0: CRITICAL OLED SILKSCREEN CHECK
Before plugging in any power, inspect the silkscreen text printed directly above the 4 pins on your **0.96" OLED display**:

```text
    VARIANT A (Most Common):            VARIANT B:
    ┌──────────────────────┐      ┌──────────────────────┐
    │ GND   VCC   SCL  SDA │      │ VCC   GND   SCL  SDA │
    └─┬─────┬─────┬─────┬──┘      └─┬─────┬─────┬─────┬──┘
      1     2     3     4           1     2     3     4
```
> 🛑 **SAFETY RULE:** **NEVER wire by pin numbers (1, 2, 3, 4). Always match the printed pin names!**  
> Reversing `VCC` and `GND` will permanently burn out the SSD1306 driver chip.

---

## 🗺️ 1. Master Connection Table

| Component | Component Pin Label | Arduino UNO Pin | Recommended Wire Color | Purpose & Notes |
| :--- | :--- | :--- | :--- | :--- |
| **0.96" OLED** | **GND** | **GND** | Black / Blue | Common Ground |
| **0.96" OLED** | **VCC** | **5V** | Red | 5V Power Supply |
| **0.96" OLED** | **SCL** (or SCK) | **A5** (or dedicated SCL) | Yellow / White | I2C Clock Line |
| **0.96" OLED** | **SDA** | **A4** (or dedicated SDA) | Green / Orange | I2C Data Line |
| **Tilt Switch** | **Leg 1** | **Digital Pin 2 (D2)** | Blue / Yellow | Jump Pack / Nudge signal |
| **Tilt Switch** | **Leg 2** | **GND** | Black | Ground (uses internal `INPUT_PULLUP`) |
| **LDR (Light)** | **Leg 1** | **5V** | Red | Power rail |
| **LDR (Light)** | **Leg 2** | **Analog Pin A0** | Purple / Orange | Analog Light Divider Node |
| **10kΩ Resistor** | **Between A0 & GND** | **GND** | Black | Pull-down resistor for LDR |
| **DHT11 Sensor** | **VCC / '+'** | **5V** | Red | 5V Power Supply |
| **DHT11 Sensor** | **GND / '-'** | **GND** | Black | Sensor Ground |
| **DHT11 Sensor** | **DATA / 'OUT' / 'S'**| **Digital Pin 4 (D4)** | Green / White | Digital Temp & Humidity Data |
| **9V Battery** | **Barrel Jack (+/-)**| **DC Power Jack** | Black Cable | Recommended onboard regulated power |

---

## 📐 2. Extra-Small Mini Breadboard Layout (170 Tie-Points)

To make your Desk Buddy sleek, portable, and presentable without a rat's nest of loose wires, use this organized placement across the 17 rows of your mini breadboard:

```text
       MINI BREADBOARD (17 Rows x 10 Columns)
       Left Side (Col A-E)         Right Side (Col F-J)
     ┌───────────────────────┬───┬───────────────────────┐
Row 1│ [OLED Pin 1: GND]     │   │                       │Row 1
Row 2│ [OLED Pin 2: VCC]     │   │                       │Row 2
Row 3│ [OLED Pin 3: SCL]     │   │                       │Row 3
Row 4│ [OLED Pin 4: SDA]     │   │                       │Row 4
Row 5│ (Spacer row)          │ C │ (Spacer row)          │Row 5
Row 6│                       │ E │ [DHT11 Pin 1: VCC / +]│Row 6
Row 7│                       │ N │ [DHT11 Pin 2: DATA/OUT│Row 7
Row 8│                       │ T │ [DHT11 Pin 3: GND / -]│Row 8
Row 9│ (Spacer row)          │ E │ (Spacer row)          │Row 9
Row10│ [LDR Leg 1 -> 5V]     │ R │                       │Row 10
Row11│ [LDR Leg 2 + 10k Res] │   │ -> Jumper to Pin A0   │Row 11
Row12│ [10k Res leg 2 -> GND]│ D │                       │Row 12
Row13│ (Spacer row)          │ I │ (Spacer row)          │Row 13
Row14│ [Tilt Switch Leg 1]   │ V │ -> Jumper to Pin 2    │Row 14
Row15│ [Tilt Switch Leg 2]   │   │ -> Jumper to GND      │Row 15
Row16│                       │   │                       │Row 16
Row17│                       │   │                       │Row 17
     └───────────────────────┴───┴───────────────────────┘
```

### Aesthetic Wiring Tips:
1. **Trimmed Jumpers / Dupont Cables:** Use short male-to-male jumpers that stay close to the board surface.
2. **Zero-Resistor Tilt Switch:** Because the Arduino UNO code enables the internal `INPUT_PULLUP` on Pin 2, you do **not** need an external resistor for the tilt switch! Just connect one leg to Pin 2 and the other to GND.
3. **Power Distribution:** Connect Arduino 5V to Row 2 / Row 6 / Row 10, and Arduino GND to Row 1 / Row 8 / Row 12 / Row 15.

---

## 🏃 3. Component Wiring Details

### A. Tilt Ball Switch ("Jump Pack" & Triple-Tap Interaction)
The tilt switch contains a tiny rolling metallic ball in a golden or black cylinder:
- **Leg 1:** Connect to Arduino **Digital Pin 2**.
- **Leg 2:** Connect to Arduino **GND**.
- **How to mount:** Mount it at a **~45° angle** (or slightly tilted) on the mini breadboard. This keeps the internal ball right on the verge of making/breaking contact, making Knoby detect gentle desk taps and light finger nudges with maximum sensitivity!
- **Gesture Controls:**
  - **3 Quick Taps on the Case / Desk:** Flips to the next page (`Pet Face` ➔ `All-in-One HUD` ➔ `Daily Quote` ➔ `Pet Face`)! Tap registration then pauses for **3 seconds** (displays `[LOCKED]`) to prevent accidental double-skips.
  - **1 Single Tap (on Face page):** Triggers Knoby's fluid, curious side-to-side inspection (smoothly glances left, double-takes right, and returns to center with an inquisitive smile and adorable blink)!
  - **Rapid Shake:** Triggers the dizzy spiral eyes (`@_@`)!
  - **Night Wake-up:** Tapping while asleep temporarily stirs the buddy awake for 12 seconds.

### B. Photoresistor (LDR - Circadian Sleep Sensor)
- **Leg 1:** Connect to **5V**.
- **Leg 2:** Connect to **Analog Pin A0**.
- **10kΩ Resistor (Brown-Black-Orange):** Insert between **Pin A0** and **GND**.
- **Day/Night Behavior:**
  - When room lights are turned OFF, Desk Buddy eyelids droop and it falls into deep sleep with floating `z Z Z` holograms.
  - Turning desk lights ON wakes the companion up with bright cheerful eyes.

### C. DHT11 Temperature & Humidity Sensor
- **Breakout Module (3 Pins: `+`, `OUT`/`S`, `-`):**
  - `+` (or VCC): Connect to **5V**
  - `-` (or GND): Connect to **GND**
  - `OUT` (or S / Signal): Connect to **Digital Pin 4**
- *(If you have the 4-pin blue package: Pin 1 = 5V, Pin 2 = Pin 4 with 10k pullup to 5V, Pin 3 = NC, Pin 4 = GND).*

---

## 🔮 4. 3D Pepper's Ghost Hologram Alignment

```text
                EYE LEVEL VIEW
                     👀
                    / 
                   /  Line of sight
                  v
         ┌───────────────────┐
         │ Transparent Glass │  <-- 45° Angle Clear Acrylic / CD Jewel Case
         │ or Rigid Plastic  │      (Projects Floating 3D Reflection)
         └─────────┬─────────┘
                   │
           [Floating 3D Pet]  <-- Illusion appears hovering in mid-air!
                   │
       ═════════════════════════
         0.96" OLED DISPLAY (Flat on base, shining upwards)
       ═════════════════════════
```

1. **The Material:** Cut a small rectangle (approx. 50mm x 50mm) of clear, rigid transparent material:
   - Clear CD jewel case cover
   - Acrylic sheet
   - Rigid clear plastic packaging or overhead projector film
2. **Mounting Angle:** Position the sheet at an exact **45-degree angle** directly above the OLED screen.
3. **Screen Orientation Setting:**
   - **Base Mount (Screen flat, facing UP):** Keep `#define SCREEN_ROTATION 0` in [`DeskBuddy.ino`](file:///Users/tofu_./Desktop/Projects/DESK%20BUDDY/Project/DeskBuddy/DeskBuddy.ino).
   - **Ceiling Mount (Screen hanging, facing DOWN):** Change to `#define SCREEN_ROTATION 2` (flips image 180° so the reflection is upright).
4. **Why OLED is essential for 3D Holograms:**
   Unlike traditional LCDs that leak gray backlight, OLED pixels turn 100% OFF for black background. Only the white glowing companion graphics emit light, making them reflect crisply on the transparent reflector with no visible screen border!

---

## 🔋 5. Portable Power Supply (9V Battery)

1. **Option A (Best): 9V Battery Snap with 2.1mm DC Barrel Jack**
   - Clip onto standard 9V alkaline battery.
   - Plug barrel connector into the Arduino UNO DC jack.
   - The Arduino UNO onboard AMS1117 linear regulator safely steps 9V down to clean 5V with built-in reverse polarity diode protection.
2. **Option B: 9V Snap with Wires (VIN & GND)**
   - **Red Wire (+9V):** Connect to Arduino **VIN** pin.
   - **Black Wire (GND):** Connect to Arduino **GND** pin.
   - ⚠️ **DANGER:** Never plug the 9V red wire into the 5V pin! Doing so bypasses the regulator and destroys the microcontroller.

---

## 🛠️ 6. Verification & Bring-Up Checklist

- [ ] Inspect OLED silkscreen: `GND` and `VCC` correctly placed.
- [ ] Tilt switch wired between D2 and GND (no external resistor needed).
- [ ] LDR divider connected to A0 with 10kΩ resistor to GND.
- [ ] DHT11 data line wired to D4.
- [ ] Open Arduino IDE -> Select **Arduino Uno** -> Select COM / Serial Port.
- [ ] Upload [`DeskBuddy.ino`](file:///Users/tofu_./Desktop/Projects/DESK%20BUDDY/Project/DeskBuddy/DeskBuddy.ino).
- [ ] Open Serial Monitor at **9600 baud** to verify time sync and sensor readings.
- [ ] Tap the breadboard to watch the **Jump Pack** spring into action!
- [ ] Cover the LDR with your thumb to verify **Sleep Mode**!
