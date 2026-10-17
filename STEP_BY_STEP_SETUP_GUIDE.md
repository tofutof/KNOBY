# 🛠️ Step-by-Step Setup Guide: 3D Hologram Desk Buddy v2.0

A complete, beginner-friendly walkthrough to build, wire, program, and assemble your **3D Hologram Desk Buddy** from scratch using your **MakerLAB Upgraded Learning Kit v2**, an **extra-small mini breadboard**, and a **0.96" Monochrome I2C OLED display**.

---

## 📦 Bill of Materials & Pre-Flight Checklist

Before you begin, gather the following components from your kit:

| Component | Quantity | Purpose |
| :--- | :--- | :--- |
| **Arduino UNO R3 Board** | 1 pc | Main microcontroller (ATmega328P) |
| **0.96" I2C Monochrome OLED (SSD1306)** | 1 pc | 128x64 high-contrast hologram projection source |
| **Extra-Small Mini Breadboard (170 Tie-Points)** | 1 pc | Compact, aesthetic prototyping base |
| **Tilt Ball Switch (Vibration Sensor)** | 1 pc | Motion sensor for Jump Pack & Triple-Tap navigation |
| **Photoresistor (LDR)** | 1 pc | Ambient light sensor for circadian auto-sleep |
| **10kΩ Resistor (Brown-Black-Orange)** | 1 pc | Pull-down resistor for the LDR light sensor |
| **DHT11 Sensor Module** | 1 pc | Temperature & relative humidity monitor |
| **Jumper Wires (Male-to-Male)** | ~10–12 pcs | Circuit interconnects (use short wires for neatness) |
| **USB Type-B Cable** | 1 pc | For uploading code and serial testing |
| **9V Battery + DC Barrel Jack Clip** | 1 pc | For standalone portable power |
| **Clear Rigid Plastic / Acrylic Sheet** | 1 pc (~50x50mm)| 45° Pepper's Ghost reflection plate (CD case cover, etc.) |

---

## ⚠️ STEP 1: The Critical OLED Silkscreen Inspection (Safety First!)

> 🛑 **WARNING: Do NOT plug in power until you inspect this!**  
> 0.96" I2C OLED modules are manufactured in two different pin layouts. Swapping `VCC` and `GND` will permanently destroy the display chip.

Look closely at the silkscreen text printed directly above the 4 pins on your physical OLED board:

```text
    VARIANT A (Most Common):            VARIANT B:
    ┌──────────────────────┐      ┌──────────────────────┐
    │ GND   VCC   SCL  SDA │      │ VCC   GND   SCL  SDA │
    └─┬─────┬─────┬─────┬──┘      └─┬─────┬─────┬─────┬──┘
      1     2     3     4           1     2     3     4
```

- If your board says **`GND - VCC - SCL - SDA`**, connect Pin 1 to **GND** and Pin 2 to **5V**.
- If your board says **`VCC - GND - SCL - SDA`**, connect Pin 1 to **5V** and Pin 2 to **GND**.
- **Always match by printed labels, never by pin numbers!**

---

## 📐 STEP 2: Placing Components on the Extra-Small Mini Breadboard

Your mini breadboard has **17 rows** divided by a center channel. To keep the device compact, aesthetic, and presentable without a rat's nest of loose wires, insert your components as mapped below:

```text
               MINI BREADBOARD (17 Rows x 10 Columns)
               Left Side (Col A–E)         Right Side (Col F–J)
             ┌───────────────────────┬───┬───────────────────────┐
Row 1        │ [OLED Pin 1: GND/VCC] │   │                       │ Row 1
Row 2        │ [OLED Pin 2: VCC/GND] │   │                       │ Row 2
Row 3        │ [OLED Pin 3: SCL]     │   │                       │ Row 3
Row 4        │ [OLED Pin 4: SDA]     │ C │                       │ Row 4
Row 5 (Gap)  │                       │ E │                       │ Row 5
Row 6        │                       │ N │ [DHT11 Pin 1: VCC / +]│ Row 6
Row 7        │                       │ T │ [DHT11 Pin 2: DATA/S] │ Row 7
Row 8        │                       │ E │ [DHT11 Pin 3: GND / -]│ Row 8
Row 9 (Gap)  │                       │ R │                       │ Row 9
Row 10       │ [LDR Leg 1 -> 5V]     │   │                       │ Row 10
Row 11       │ [LDR Leg 2 + 10k Res] │ D │ ──> Jumper to Pin A0  │ Row 11
Row 12       │ [10k Res Leg 2 -> GND]│ I │                       │ Row 12
Row 13 (Gap) │                       │ V │                       │ Row 13
Row 14       │ [Tilt Switch Leg 1]   │ I │ ──> Jumper to Pin 2   │ Row 14
Row 15       │ [Tilt Switch Leg 2]   │ D │ ──> Jumper to GND     │ Row 15
Row 16       │                       │ E │                       │ Row 16
Row 17       │                       │ R │                       │ Row 17
             └───────────────────────┴───┴───────────────────────┘
```

### Component Placement Instructions:
1. **OLED Display:** Insert the 4-pin header into **Rows 1, 2, 3, and 4** on the left side.
2. **DHT11 Module:** Insert the 3-pin module into **Rows 6, 7, and 8** on the right side.
3. **Photoresistor (LDR):** Insert one leg into **Row 10** and the other leg into **Row 11**.
4. **10kΩ Resistor:** Insert one leg into **Row 11** (sharing the row with the LDR's second leg) and the other leg into **Row 12**.
5. **Tilt Ball Switch:** Insert into **Rows 14 and 15**. Mount it upright (or at a slight 30° angle). *(Note: Because Arduino Uno enables internal `INPUT_PULLUP` on Pin 2, the tilt switch needs zero external resistors!)*

---

## 🔌 STEP 3: Wiring Jumpers to the Arduino UNO

Make the point-to-point connections listed in the table below:

| From Breadboard Row | Component & Pin | To Arduino UNO Header Pin | Wire Color Recommendation |
| :--- | :--- | :--- | :--- |
| **Row 1** | OLED GND *(or VCC if Variant B)* | **GND** | Black |
| **Row 2** | OLED VCC *(or GND if Variant B)* | **5V** | Red |
| **Row 3** | OLED SCL | **A5** *(or dedicated top SCL pin)* | Yellow or White |
| **Row 4** | OLED SDA | **A4** *(or dedicated top SDA pin)* | Green or Orange |
| **Row 6** | DHT11 VCC (`+`) | **5V** | Red |
| **Row 7** | DHT11 DATA (`OUT` / `S`) | **Digital Pin 4** | Green or Blue |
| **Row 8** | DHT11 GND (`-`) | **GND** | Black |
| **Row 10**| LDR Leg 1 | **5V** | Red |
| **Row 11**| LDR Leg 2 / Resistor junction | **Analog Pin A0** | Purple or Orange |
| **Row 12**| 10kΩ Resistor Ground leg | **GND** | Black |
| **Row 14**| Tilt Ball Switch Leg 1 | **Digital Pin 2** | Blue |
| **Row 15**| Tilt Ball Switch Leg 2 | **GND** | Black |

> 💡 **Tidy Wiring Tip:** Keep your jumper wires short and pressed close to the breadboard surface. This gives your build a clean "commercial gadget" appearance rather than a messy prototype!

---

## 💻 STEP 4: Setting Up Arduino IDE & Libraries

1. **Install Arduino IDE:** Download and open the latest version of Arduino IDE (v2.x) from [arduino.cc](https://www.arduino.cc/en/software).
2. **Connect Arduino UNO:** Plug your Arduino UNO into your computer using the USB cable.
3. **Install Required Libraries:**
   - In Arduino IDE, click the **Library Manager** icon on the left sidebar (or go to `Tools -> Manage Libraries...`).
   - Search for **`Adafruit SSD1306`** and click **Install** (select *"Install All"* if prompted to include dependencies).
   - Search for **`Adafruit GFX Library`** and verify it is installed.
   - *(Note: The DHT11 temperature/humidity driver is custom-built directly into `DeskBuddy.ino`, so you do **not** need to install any external DHT library!)*
4. **Select Board & Port:**
   - Go to `Tools -> Board -> Arduino AVR Boards -> Arduino Uno`.
   - Go to `Tools -> Port` and select your Arduino serial port (e.g., `/dev/cu.usbserial...` on Mac or `COM3`/`COM4` on Windows).

---

## 🚀 STEP 5: Uploading the Firmware

1. In Arduino IDE, open:
   [`Project/DeskBuddy/DeskBuddy.ino`](file:///Users/tofu_./Desktop/Projects/DESK%20BUDDY/Project/DeskBuddy/DeskBuddy.ino)
2. Click the **Upload** button (the arrow icon in the top toolbar).
3. Wait for the bottom status console to say **"Done uploading"**.
4. **Watch the OLED display!** Within 2 seconds, you should see the **`DESK BUDDY v2.0`** splash screen followed by the live companion face.
5. *(Optional)* Open the **Serial Monitor** (`Tools -> Serial Monitor`) at **9600 baud** to see real-time clock synchronization confirmation and sensor status messages.

---

## 🔮 STEP 6: Assembling the 3D Pepper's Ghost Hologram Reflector

Pepper's Ghost is the optical technique used in theme parks and high-end hologram displays. Because black OLED pixels emit zero light, only the glowing white graphics reflect off the clear plate, making them appear to float in mid-air!

```text
               EYE LEVEL LINE OF SIGHT
                         👀
                        /
                       /
                      v
             ┌─────────────────┐
             │ 45° Clear Glass │   <-- Rigid clear acrylic or CD jewel case
             │ or Plastic Plate│
             └────────┬────────┘
                      │
              [ Floating 3D Pet ]  <-- Appears suspended in mid-air!
                      │
         ═══════════════════════════
           0.96" OLED DISPLAY (Flat on base, shining upwards)
         ═══════════════════════════
```

1. **Prepare the Reflector:** Cut a small rectangle (approximately 50mm × 50mm) from a clear CD jewel case cover, clear acrylic sheet, or rigid overhead projector transparency.
2. **Mount at 45 Degrees:** Prop the clear sheet at an exact **45-degree angle** directly above the OLED screen.
3. **If the Reflected Image Appears Upside-Down:**
   In [`DeskBuddy.ino`](file:///Users/tofu_./Desktop/Projects/DESK%20BUDDY/Project/DeskBuddy/DeskBuddy.ino#L48), change:
   ```cpp
   #define SCREEN_ROTATION 0  // Change to 2 to flip 180° for top-mounted setups
   ```
   and click Upload again.
4. **Best Viewing Experience:** Dim your desk lights slightly. The floating hologram effect will pop out with stunning 3D contrast!

---

## 🎮 STEP 7: Interactive Testing & Gestures

Once powered on, test all the interactive behaviors:

| Gesture / Event | How to Test | Expected Reaction |
| :--- | :--- | :--- |
| **Living Face Behavior** | Watch the screen while in Page 1 | The eyes gently breathe (micro-drifts of ±2px), blink with 4-phase fluid motion, and the mouth smiles or tilts inquisitively. |
| **1 Single Tap (Tier 1: Curious ?)** | Gently tap the case or table once | Desk Buddy displays a curious questioning gaze with a floating animated `?` emote (starts a 5-second combo window). |
| **Tap Again within 5s (Tier 2: Little Angry)** | Tap once again within 5 seconds | Desk Buddy glares grumpily with slanted eyebrows (`\ /`), a downward pout, and an annoyed cheek steam puff (`oO`). |
| **Tap Again within 5s (Tier 3: Super Angry)** | Tap once again within 5 seconds | Full anime rage mode! Jittering fury tremble, roaring shouting mouth with teeth, and a throbbing anime anger vein mark (`💢`). |
| **Cool Down (5s Inactivity)** | Don't touch for 5 seconds | Automatically resets anger back to 0 and returns to peaceful companion mode. |
| **3 Quick Taps (Page Flip)** | Knock or tap the case 3 times quickly | Advances to the next screen:<br>• **Page 1:** Xiaozhi Living Pet Face<br>• **Page 2:** All-in-One Dashboard HUD (Clock + Date + Temp + Humid + Light)<br>• **Page 3:** Daily Wisdom Quote (with talking animated mouth) |
| **Room Lights Off (Sleep)** | Turn off desk lights or cover LDR with thumb | The buddy's eyelids droop into peaceful closed eyes (`- -`) and animated **3D hologram `z Z Z`** symbols float upward! |
| **Room Lights On (Wake)** | Turn lights back on or uncover LDR | The companion wakes up automatically with an excited stretch and bright happy smile. |

---

## 🔋 STEP 8: Portable 9V Battery Power (Desk Ready!)

To display your Desk Buddy without needing your computer:

1. Connect the **9V Battery Snap with 2.1mm DC Barrel Jack** to a standard 9V battery.
2. Plug the barrel connector into the Arduino UNO's black DC power jack.
3. **Why this is safe:** The DC barrel jack routes power through the Arduino UNO's onboard linear voltage regulator (AMS1117) and built-in reverse-polarity protection diode, stepping the 9V cleanly and safely down to 5V.
4. ⚠️ **Safety Warning:** Never connect the red 9V battery wire directly into the Arduino's 5V pin! Doing so bypasses the regulator and will destroy the ATmega328P chip.

---

## ❓ Troubleshooting FAQ

### 1. The OLED display remains completely dark
- **Check Silkscreen:** Double-check whether your display is Variant A (`GND-VCC-SCL-SDA`) or Variant B (`VCC-GND-SCL-SDA`).
- **Check I2C Address:** Most SSD1306 displays use address `0x3C`. If yours uses `0x3D`, change line 39 in `DeskBuddy.ino` to `#define SCREEN_ADDRESS 0x3D`.
- **Check I2C Pins:** Ensure SCL connects to **A5** and SDA connects to **A4**.

### 2. Taps are not registering or too sensitive
- In `DeskBuddy.ino`, locate lines 58–59:
  ```cpp
  const unsigned long MIN_TAP_INTERVAL = 75;  // Increase to 90 if getting false double-taps
  const unsigned long MULTI_TAP_WINDOW = 850; // Increase to 1000 if 3 taps feel too rushed
  ```
- Make sure the tilt switch is seated firmly in the breadboard rows so its legs don't wobble.

### 3. Temperature or Humidity shows 25.5°C / 58% fallback values
- Verify that the DHT11's `+` pin is in **5V**, `-` pin is in **GND**, and the `OUT` / `DATA` pin is plugged firmly into Arduino **Digital Pin 4**.

---

## 📄 Generating Your Final Case Study Report

When your physical prototype is finished and you have taken photos:
1. Place a photo of your build into [`Case_Study_Prompter/assets/finished_project_1.jpg`](file:///Users/tofu_./Desktop/Projects/DESK%20BUDDY/Case_Study_Prompter/assets).
2. Run the automated case study builder in your terminal:
   ```bash
   python3 Case_Study_Prompter/prompter.py --build
   ```
3. Open the publication-ready PDF:
   ```bash
   open Case_Study_Prompter/Final_Case_Study.pdf
   ```

🎉 **Congratulations! Your 3D Hologram Desk Buddy v2.0 is complete and ready to guard your desk!**
