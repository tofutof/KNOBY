# 💻 Desk Buddy v2.0: Interactive Hologram Companion Firmware

This folder contains the complete firmware and wiring architecture for the **Desk Buddy v2.0** desktop companion.

> 📘 **Looking for the complete beginner build walkthrough?** See the full [`docs/STEP_BY_STEP_SETUP_GUIDE.md`](../docs/STEP_BY_STEP_SETUP_GUIDE.md) in the `docs/` folder!

---

## 📁 Directory Structure
```text
DeskBuddy/
├── DeskBuddy.ino       # Main Arduino IDE sketch file (Triple-Tap & All-in-One HUD)
└── README.md           # Quick upload and configuration guide
```

---

## 🌟 Interactive Features & Hardware Capabilities
1. **🤖 Procedural Animated Face Engine (Xiaozhi-Inspired):**
   - **Organic Saccade Drifts:** Eyes and mouth subtly micro-shift (±2px X, ±1px Y) every few seconds to create lifelike breathing.
   - **Multi-Phase Fluid Blinking:** 4-stage smooth squash-and-stretch eye curve (Open ➔ Squash 6px ➔ Slit 2px ➔ Rebound 10px ➔ Open).
   - **Dynamic Animated Mouth:** Cute resting smile bar that tracks saccades, curious head-tilt smirks, and animated talking/chattering mouth that pulses when reciting daily quotes!
   - **Interactive Expressions:** Winks, heart eyes, joyful arches, plus a **3-Tier Annoyance Escalation** on single taps:
     - **Tap 1:** Curious Question Mark (`?`) emote (opens 5s combo window)
     - **Tap 2 (within 5s):** "Little Angry" face (`\ /` glare, pout, side cheek steam puff)
     - **Tap 3 (within 5s):** "Super Angry" face (fury jitter, yelling mouth with teeth, throbbing anime `💢` vein)
     - **Calm Down:** Cools down completely back to calm companion mode if left alone for 5 seconds!
2. **👉 Triple-Tap Navigation (Tilt Ball Switch on Pin 2):**
   - **3 Quick Taps on the Case / Desk:** Instantly advances to the next page!
     - **Page 1: Companion Face (Xiaozhi-Inspired)** (Living breathing pet)
     - **Page 2: All-in-One Dashboard HUD** (Real-time Clock, Calendar Date, Temp, Humidity, Light level)
     - **Page 3: Daily Wisdom Quote** (Flash `PROGMEM` quotes with speaking buddy mouth)
2. **📊 All-in-One Dashboard HUD:**
   - Combines the **Real-Time Clock** (bold, centered `HH:MM:SS`), **Calendar Date** (`OCT 12, 2026`), **Comfort Badge** (`[COMFY]`), **Live Temperature** (`TEMP: 26.5°C`), **Relative Humidity** (`HUM: 58%`), and **Room Light** status (`LIGHT: BRIGHT`) on a single glanceable page!
3. **🌙 Circadian Auto-Sleep Engine (Photoresistor LDR on Pin A0):**
   - Monitors ambient room lighting in real-time.
   - When room lights are turned OFF, the buddy yawns, eyelids droop, and it enters deep sleep with peaceful closed eyes and floating 3D hologram `z Z Z` animations.
   - When room lights are turned ON, it automatically wakes up with an excited stretch and smile! (A physical tap also stirs it awake temporarily).
4. **🌡️ Built-in Zero-Dependency DHT11 Driver (Pin 4):**
   - Direct non-blocking bit-bang decoder reading ambient temperature and relative humidity with zero third-party library conflicts.
5. **🔮 3D Pepper's Ghost Hologram Alignment:**
   - Optimized high-contrast pure black background (`SSD1306_BLACK`).
   - Configurable orientation (`#define SCREEN_ROTATION 0` for flat base reflector, `2` for top-mounted inverted reflector).

---

## 🚀 How to Flash via Arduino IDE
1. Open the **Arduino IDE**.
2. Go to **File -> Open...** and select:
   [`DeskBuddy.ino`](DeskBuddy.ino)
3. Under **Tools -> Board**, select **Arduino Uno**.
4. Under **Tools -> Port**, select your Arduino USB Serial Port.
5. In **Tools -> Manage Libraries...**, ensure the following are installed:
   - **Adafruit SSD1306**
   - **Adafruit GFX Library**
   *(Note: The DHT11 reader is fully built-in, requiring no additional DHT library!)*
6. Click **Upload** (Arrow icon).

---

## ⚙️ Configuration Options
In [`DeskBuddy.ino`](DeskBuddy.ino):
- **Hologram Reflection Orientation:**
  ```cpp
  #define SCREEN_ROTATION 0 // Set to 2 if reflection appears upside down
  ```
- **Triple-Tap Timing Window:**
  ```cpp
  const unsigned long MIN_TAP_INTERVAL = 75;  // Debounce contact chatter (ms)
  const unsigned long MULTI_TAP_WINDOW = 850; // Window to complete 3 taps (ms)
  ```
- **LDR Light Thresholds:**
  ```cpp
  const int LDR_DARK_THRESHOLD  = 160; // Dark room sleep trigger
  const int LDR_LIGHT_THRESHOLD = 240; // Lights on wake trigger
  ```
