# 📄 Final Case Study Submission Template: Hologram Desk Buddy

> **Instructions:** Fill out the sections below. You can either edit this file or update `case_study_data.json` and run `python3 prompter.py --build` to automatically produce `Final_Case_Study.pdf`.

---

### TYPE MEMBERS NAME
- Rairolf Rabang
- Nicole Margareth Sibal

---

### 1. Updated image of circuit design (WOKWI/Tinkercad simulation if any/drawing/diagram)
- **Simulation / Design Link:** [Insert Wokwi or Tinkercad URL here]
- **Image Attachment:** Place image in `assets/circuit_diagram.png`
- **Brief Circuit Notes:** Arduino UNO powered by 9V battery via VIN/GND (or DC Barrel Jack), connected to 0.96" SSD1306 OLED via I2C (SDA -> A4, SCL -> A5, VCC -> 5V, GND -> GND). Display projects onto a 45° angled transparent acrylic reflector (Pepper's Ghost hologram effect). Optional LM35/TMP36 analog temp sensor connected to A0.

---

### 2. Picture/s of the finished project
- **Finished Project Photos:** Place image(s) in `assets/finished_project_1.jpg`
- **Assembly Description:** Enclosure with 45° transparent pyramid/reflector prism, housing the Arduino UNO, 9V battery clip, and mounted OLED screen projecting floating animated faces, clock, temperature, and quotes.

---

### 3. List of materials/components/wiring config

#### A. Bill of Materials
| Component / Part | Quantity | Specifications / Purpose |
| :--- | :--- | :--- |
| Arduino UNO R3 | 1 | Main ATmega328P microcontroller |
| 0.96" I2C OLED Display | 1 | 128x64 Monochrome display (SSD1306) |
| Transparent Acrylic / Plastic Prism | 1 | 45-degree Pepper's Ghost beam reflector |
| 9V PP3 Alkaline Battery | 1 | Standalone portable power source |
| 9V Battery Snap with 2.1mm DC Jack | 1 | Connects battery to Arduino DC jack |
| Mini Breadboard & Jumpers | 1 set | Wiring connections (I2C & Power) |
| LM35 / TMP36 Temp Sensor (Optional) | 1 | Analog temperature sensing on pin A0 |
| SPST Power Switch (Optional) | 1 | On/off toggle switch |

#### B. Wiring Configuration
| Source Pin | Arduino UNO Pin | Function |
| :--- | :--- | :--- |
| OLED GND | GND | Ground |
| OLED VCC | 5V | 5V DC Power |
| OLED SCL | A5 (SCL) | I2C Clock Line |
| OLED SDA | A4 (SDA) | I2C Data Line |
| Temp Sensor VOUT (Optional) | A0 | Analog Temperature Input |
| 9V Battery (+) | VIN (or DC Jack Center) | +9V Input |
| 9V Battery (-) | GND (or DC Jack Outer) | Power Return Ground |

---

### 4. Estimated cost of the project components used

| Item Description | Unit Cost | Total Cost |
| :--- | :--- | :--- |
| Arduino UNO R3 Board | $7.50 / ₱420.00 | $7.50 / ₱420.00 |
| 0.96" I2C OLED Display | $2.50 / ₱140.00 | $2.50 / ₱140.00 |
| Transparent Acrylic Hologram Reflector | $1.20 / ₱70.00 | $1.20 / ₱70.00 |
| 9V Alkaline Battery | $1.50 / ₱85.00 | $1.50 / ₱85.00 |
| 9V Battery Snap Clip with DC Jack | $0.75 / ₱40.00 | $0.75 / ₱40.00 |
| Breadboard & Jumper Wires | $1.80 / ₱100.00 | $1.80 / ₱100.00 |
| LM35 Temp Sensor (Optional) | $0.80 / ₱45.00 | $0.80 / ₱45.00 |
| **Total Estimated Cost:** | | **$16.05 (Approx. ₱900.00)** |

---

### 5. Full working code
See [`Project/DeskBuddy/DeskBuddy.ino`](../Project/DeskBuddy/DeskBuddy.ino).

```cpp
// Full working code is automatically pulled from Project/DeskBuddy/DeskBuddy.ino
// when building the PDF via prompter.py
```

---

### 6. Short video test (can be in link)
- **Video Link:** [Insert your YouTube, Google Drive, or Loom link here]
- **Demonstration Highlights:** 3D floating hologram reflection on the 45-degree prism, carousel cycling through animated companion faces, real-time digital clock, ambient temperature, and daily quote.

---

### 7. Short description of the project functionality
The Hologram Desk Buddy is a compact, interactive desktop companion engineered using an Arduino UNO and a 0.96-inch monochrome I2C OLED display (128x64 pixels). By leveraging the optical Pepper's Ghost principle with a 45-degree transparent reflection plate, the system projects images that appear to float seamlessly in 3D space. The device cycles smoothly between four dynamic modes: (1) expressive animated companion faces (happy, blinking, looking, winking, heart eyes, and sleepy), (2) a real-time digital clock and date display with Serial synchronization, (3) live ambient temperature readout in both Celsius and Fahrenheit with a graphical thermometer gauge, and (4) a curated Quote of the Day stored in Flash PROGMEM to conserve precious SRAM. Completely standalone and powered by a 9V battery with optimized frame pacing, the Desk Buddy delivers an engaging, functional, and futuristic enhancement to any workstation.
