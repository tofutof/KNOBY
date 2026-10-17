# Contributing to 3D Hologram Desk Buddy v2.0 🔮

First off, thank you for considering contributing to **Desk Buddy v2.0**! Whether you're fixing a bug, adding new facial animations, enhancing sensor drivers, or improving the documentation, your help is welcome.

---

## 📜 Code of Conduct

This project and everyone participating in it is governed by our [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior to `raifrabang@gmail.com`.

---

## 🛠️ How Can I Contribute?

### 1. Reporting Bugs
- Check the [Issues](../../issues) tab first to ensure the bug hasn't already been reported.
- If not, create a new issue using our **Bug Report** template.
- Include:
  - Microcontroller board used (e.g., Arduino Uno R3, Uno SMD, Nano).
  - Sensor versions and wiring pin allocations.
  - Arduino IDE version and library versions (`Adafruit SSD1306`, `Adafruit GFX`).
  - Clear steps to reproduce the issue and observed vs expected behavior.

### 2. Suggesting Features & Animations
- We love cute animations, new mini-games, and sensor expansions!
- Open a **Feature Request** issue explaining:
  - What the feature does.
  - Why it enhances the Desk Buddy experience.
  - Expected hardware / RAM impact.

### 3. Submitting Pull Requests (PRs)
1. **Fork** the repository.
2. Create a topic branch: `git checkout -b feature/my-new-animation`.
3. Test your code thoroughly on physical hardware or a Wokwi simulation.
4. Keep commits small, atomic, and well-described:
   - `feat: add sleepy yawn animation`
   - `fix: debounce tilt sensor trigger on rapid desk taps`
   - `docs: update wiring pinout diagram`
5. Push to your fork: `git push origin feature/my-new-animation`.
6. Open a **Pull Request** against the `main` branch.

---

## ⚡ Arduino Uno (ATmega328P) Development Guidelines

Because the Arduino Uno R3 has strictly **32 KB Flash** and **only 2 KB SRAM**, all contributions must observe embedded best practices:

1. **Protect Dynamic Memory (SRAM):**
   - **Never use dynamic `String` objects.** Use standard C strings (`char[]`) or fixed buffers.
   - Always wrap string literals in flash memory using the `F()` macro: `Serial.println(F("Hello!"));` or `display.print(F("Text"));`.
   - Store large lookup tables, sprite bitmaps, and quote lists in `PROGMEM`.

2. **Non-Blocking Architecture:**
   - Avoid long `delay()` calls in the main loop to keep the animation and tilt interrupt response smooth.
   - Use `millis()`-based state machines and timer comparisons.

3. **Sensor Safety & Pin Conventions:**
   - Keep default pins standard unless configurable via `#define`:
     - Pin 2: Tilt Ball Switch (`INPUT_PULLUP`)
     - Pin 4: DHT11 Sensor
     - Pin A0: Photoresistor (LDR)
     - Pin A4/A5: I2C OLED (SDA/SCL)

---

## ⚖️ License
By contributing to this repository, you agree that your contributions will be licensed under the project's [MIT License](LICENSE).
