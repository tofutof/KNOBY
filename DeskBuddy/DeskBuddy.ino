/*
 * =====================================================================
 *  KNOBY v2.0 - 3D HOLOGRAM DESK BUDDY
 *  Interactive Procedural Face Engine & Multi-Sensor Companion
 *  (Visual design inspired by the Xiaozhi AI aesthetic)
 * =====================================================================
 *  Authors:         Rairolf Rabang & Nicole Margareth Sibal
 *  License:         MIT License (Copyright (c) 2026 Rairolf Rabang & Nicole Margareth Sibal)
 *  Microcontroller: Arduino UNO R3 (MakerLAB Upgraded Learning Kit v2)
 *  Display:         0.96" I2C Monochrome OLED (SSD1306, 128x64)
 *  Optics:          45° Transparent Pepper's Ghost Hologram Reflector
 *  Sensors:         - Tilt Ball Switch (Pin 2: Tap Combo Escalation & Triple-Tap)
 *                   - Photoresistor LDR (Pin A0: Circadian Auto-Sleep)
 *                   - DHT11 Sensor (Pin 4: Live Temp & Humidity)
 *  Form Factor:     Extra-Small Mini Breadboard (170 Tie-Points)
 *  Power:           9V Battery / USB 5V
 *
 *  PROCEDURAL FACE ENGINE FEATURES (Inspired by Xiaozhi aesthetic, custom-built for Arduino Uno SSD1306):
 *    1. FLUID IDLE GAZE ENGINE: Liquid smooth Lerp gliding across the display!
 *       The eyes actively inspect their surroundings (glance left -13px, glance
 *       right +13px, look up curiously -7px, diagonal glances, and modest downward rest)
 *       with organic hold durations (1.5-3.0s) and seamless return-to-center.
 *    2. PROGRESSIVE SQUASH-AND-STRETCH BLINKING: Smooth frame-by-frame eyelid
 *       interp curve with natural blinks coordinated during wide gaze transitions.
 *    3. 3D PUPIL PARALLAX & GLOSS: Black pupils lead the gaze with an adorable
 *       white micro-reflection highlight dot for that glossy holographic shine.
 *    4. CONNECTED 3D MOUTH PERSPECTIVE: The mouth smoothly tracks the gaze direction
 *       with dampened parallax, making the entire face feel like a real turning head!
 *    5. 3D ARCADE COCKPIT & VICTORY WINK (Single Tap):
 *       Tapping the desk instantly launches a 3D first-person retro arcade cockpit racer!
 *       Features smooth parabolic S-curve drifting, tilting F1 sports steering wheel with Knoby's paws,
 *       rearview mirror, nitro boost, and a cinematic camera pan-out to Knoby winking
 *       with a sparkling anime star on the side (^ ~)✦!
 *    6. CIRCADIAN AUTO-SLEEP: Turning room lights off gracefully droops
 *       eyelids into deep sleep with floating 3D hologram "zZz"!
 *    7. TRIPLE-TAP GESTURE NAVIGATION:
 *       [Page 1: Pet Face] -> [Page 2: All-in-One HUD] -> [Page 3: Daily Quote]
 *
 *  REQUIRED LIBRARIES (Arduino Library Manager):
 *    - Adafruit SSD1306
 *    - Adafruit GFX Library
 *    - Wire (built-in)
 *
 *  PIN ALLOCATIONS:
 *    - OLED GND   -> GND
 *    - OLED VCC   -> 5V
 *    - OLED SCL   -> A5 (or top SCL)
 *    - OLED SDA   -> A4 (or top SDA)
 *    - TILT SW    -> Pin 2 & GND (Uses internal INPUT_PULLUP, 0 resistors!)
 *    - LDR (A0)   -> 5V -> LDR -> Pin A0 -> 10k Resistor -> GND
 *    - DHT11 DATA -> Pin 4 (VCC to 5V, GND to GND)
 * =====================================================================
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

// Set to 1 if you ever need live USB Serial monitor debugging, 0 for production release (saves ~1.4 KB flash!)
#define ENABLE_SERIAL_DEBUG 0

// =====================================================================
//  HARDWARE PIN DEFINITIONS
// =====================================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

constexpr int PIN_TILT = 2;    // Tilt ball switch (to GND with INPUT_PULLUP)
constexpr int PIN_DHT  = 4;    // DHT11 Temperature & Humidity Data
constexpr int PIN_LDR  = A0;   // Photoresistor analog voltage divider

// =====================================================================
//  OPTICAL HOLOGRAM CONFIGURATION
// =====================================================================
// 0 = Normal (Screen flat on base facing UP into 45° reflector)
// 2 = 180° Inverted (Screen mounted on ceiling facing DOWN into reflector)
#define SCREEN_ROTATION 0

// Invert horizontally for optical hologram reflector (mirrors X-axis so reflection is readable)
#define HOLOGRAM_HORIZONTAL_MIRROR true

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =====================================================================
//  LIGHT SENSOR (LDR) THRESHOLDS
// =====================================================================
const int LDR_DARK_THRESHOLD  = 160; // Below this = Dark room (Sleep)
const int LDR_LIGHT_THRESHOLD = 240; // Above this = Lights On (Awake)
bool isRoomDark = false;
unsigned long awakeTemporaryUntil = 0; // Temporary wake on tap during dark

// Smooth Sleep & Wake State Machine
enum SleepState {
  SLEEP_AWAKE,          // Fully awake
  SLEEP_FALLING_ASLEEP, // Cinematic gradual eyelid droop & heavy tiredness
  SLEEP_DEEP_ASLEEP,    // Peaceful closed eyes with floating 'z' holograms
  SLEEP_WAKING_UP       // Gentle flutter-open wake transition
};

SleepState currentSleepState = SLEEP_AWAKE;
unsigned long sleepTransitionStartTime = 0;
const unsigned long FALLING_ASLEEP_DURATION = 3200; // ms (yawn, double blink & fluid crescent morph)
const unsigned long WAKING_UP_DURATION      = 3000; // ms (fluid 3.0s cute cinematic wake-up with morphing cheerful bounce)

// =====================================================================
//  TILT SWITCH, TRIPLE-TAP & MINI KART ENGINE (High Sensitivity)
// =====================================================================
int lastTiltState = HIGH;
unsigned long lastTapRegisteredTime = 0;
const unsigned long MIN_TAP_INTERVAL          = 25;   // Ultra-sensitive debounce (ms)
const unsigned long SINGLE_TAP_RESOLVE_WINDOW = 240;  // Ultra-responsive single tap resolve (ms)
const unsigned long TRIPLE_TAP_COOLDOWN       = 1400; // Snappy lockout after page switch (ms)
unsigned long tapCooldownUntil = 0;

int tapCount = 0;
unsigned long firstTapTime = 0;

// Hardware Interrupt flag for Pin 2 (INT0)
volatile bool tiltTriggered = false;
volatile unsigned long isrLastTriggerMicros = 0;

void tiltISR() {
  unsigned long m = micros();
  // 25ms microsecond debounce inside ISR to absorb mechanical ball chatter
  if (m - isrLastTriggerMicros > 25000UL) {
    isrLastTriggerMicros = m;
    tiltTriggered = true;
  }
}

// =====================================================================
//  3D ARCADE COCKPIT RACER REACTION (Single Tap Trigger)
// =====================================================================
bool isDrivingKart = false;
unsigned long kartStartTime = 0;
const unsigned long KART_TOTAL_DURATION = 2600; // ms for full 3D arcade cockpit & victory wink animation

// =====================================================================
//  XIAOZHI-STYLE FACE ENGINE (Saccades, Fluid Blinking, Animated Mouth)
// =====================================================================
const int BASE_EYE_H = 28;     // Standard eye height
const int BASE_EYE_W = 19;     // Standard eye width (H * 0.65)
const int EYE_RADIUS = 5;      // Smooth rounded corners
const int LEFT_EYE_CX = 36;    // Left eye X center
const int RIGHT_EYE_CX = 92;   // Right eye X center
const int BASE_EYE_CY = 27;    // Perfectly centered Eye Y center line (64px OLED)

// Fluid Eye Motion Engine (Smooth Gliding & Expressive Idle Gaze)
int currentEyeX = 0;
int currentEyeY = 0;
int targetEyeX  = 0;
int targetEyeY  = 0;
unsigned long nextGazeChangeTime = 0;

// Fluid Progressive Blinking State Machine (Squash & Stretch)
int blinkPhase = 0;
unsigned long lastBlinkPhaseChange = 0;
unsigned long nextBlinkTriggerTime = 3000;
int currentEyeHeight = BASE_EYE_H;

// Dynamic Mouth Engine
enum MouthMode {
  MOUTH_IDLE,       // Resting cute horizontal smile bar
  MOUTH_LISTENING,  // Asymmetric curious slit
  MOUTH_SPEAKING,   // Animated talking/chattering pulse
  MOUTH_OPEN_SMILE  // Happy open smile
};

MouthMode currentMouthMode = MOUTH_IDLE;
int currentMouthHeight = 4;
int targetMouthHeight  = 4;
unsigned long lastMouthUpdateTime = 0;

// Alternate Moods (Happy, Questioning '?', Idea '!', Wink, Heart Eyes, Curious)
enum CompanionMood {
  MOOD_NORMAL_LIVING, // Xiaozhi organic breathing, blinks & mouth
  MOOD_QUESTIONING,   // Puzzled head tilt, asymmetric squint & floating '?' above head
  MOOD_IDEA_SPARK,    // Aha! moment with floating '!' and sparkle rays
  MOOD_CURIOUS_TILT,  // Inquisitive head tilt / asymmetric eyes
  MOOD_WINK,          // Playful wink
  MOOD_HEART_EYES,    // Love / heart eyes
  MOOD_EXCITED_ARCHES // Cheerful joyful curves (^ ^)
};

CompanionMood currentMood = MOOD_NORMAL_LIVING;
unsigned long lastMoodChange = 0;
unsigned long moodDuration = 3500;

int zDriftOffset = 0; // Floating 'z' hologram animation offset

// =====================================================================
//  DHT11 TEMPERATURE & HUMIDITY DRIVER (Direct Bit-Bang, Non-Blocking)
// =====================================================================
struct DHTData {
  int temperatureC;
  int humidity;
  bool valid;
};

DHTData lastDHTReading = {25, 56, false};
unsigned long lastDHTPollTime = 0;
const unsigned long DHT_POLL_INTERVAL = 2000;

bool readDHT11Raw(int &tempC, int &humidity) {
  uint8_t data[5] = {0, 0, 0, 0, 0};

  // Step 1: Send Start Signal
  pinMode(PIN_DHT, OUTPUT);
  digitalWrite(PIN_DHT, LOW);
  delay(18); // 18ms LOW pulse
  digitalWrite(PIN_DHT, HIGH);
  delayMicroseconds(30);
  pinMode(PIN_DHT, INPUT_PULLUP);

  // Step 2: Await DHT11 response (80us LOW, 80us HIGH)
  unsigned long timeout = micros();
  while (digitalRead(PIN_DHT) == HIGH) {
    if (micros() - timeout > 100) return false;
  }
  timeout = micros();
  while (digitalRead(PIN_DHT) == LOW) {
    if (micros() - timeout > 100) return false;
  }
  timeout = micros();
  while (digitalRead(PIN_DHT) == HIGH) {
    if (micros() - timeout > 100) return false;
  }

  // Step 3: Read 40 bits
  for (int i = 0; i < 40; i++) {
    timeout = micros();
    while (digitalRead(PIN_DHT) == LOW) {
      if (micros() - timeout > 100) return false;
    }

    unsigned long pulseStart = micros();
    timeout = micros();
    while (digitalRead(PIN_DHT) == HIGH) {
      if (micros() - timeout > 100) return false;
    }
    unsigned long pulseLength = micros() - pulseStart;

    int byteIdx = i / 8;
    data[byteIdx] <<= 1;
    if (pulseLength > 40) {
      data[byteIdx] |= 1;
    }
  }

  // Step 4: Checksum Check
  if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
    humidity = data[0];
    tempC    = data[2];
    return true;
  }
  return false;
}

void updateDHT11() {
  unsigned long now = millis();
  if (now - lastDHTPollTime >= DHT_POLL_INTERVAL) {
    lastDHTPollTime = now;
    int t, h;
    if (readDHT11Raw(t, h)) {
      lastDHTReading.temperatureC = t;
      lastDHTReading.humidity = h;
      lastDHTReading.valid = true;
    } else if (!lastDHTReading.valid) {
      lastDHTReading.temperatureC = 25;
      lastDHTReading.humidity = 58;
    }
  }
}

// =====================================================================
//  REAL-TIME CLOCK (RTC) SYSTEM
// =====================================================================
int currentHours = 0;
int currentMinutes = 0;
int currentSeconds = 0;
int currentDay = 12;
int currentMonth = 10;
int currentYear = 2026;
unsigned long lastClockTick = 0;

enum DayPeriod {
  PERIOD_MORNING,   // 06:00 - 11:59
  PERIOD_AFTERNOON, // 12:00 - 17:59
  PERIOD_EVENING,   // 18:00 - 21:59
  PERIOD_NIGHT      // 22:00 - 05:59
};

DayPeriod currentPeriod = PERIOD_MORNING;

// =====================================================================
//  ROTATING QUOTES (Stored in Flash PROGMEM to protect SRAM)
// =====================================================================
const char quote_0[] PROGMEM = "Stay curious and keep tinkering.";
const char quote_1[] PROGMEM = "Believe in yourself and build.";
const char quote_2[] PROGMEM = "Small steps lead to big results.";

const char* const quotesTable[] PROGMEM = {
  quote_0, quote_1, quote_2
};
const int totalQuotes = 3;
int currentQuoteIndex = 0;

// =====================================================================
//  PAGE MODES (Controlled via Triple-Tap)
// =====================================================================
enum DisplayMode {
  MODE_FACE_ANIMATION,  // Page 1: Xiaozhi Interactive Living Face
  MODE_ALL_IN_ONE_HUD,  // Page 2: Combined Clock, Date, Temp & Humidity
  MODE_QUOTE_DISPLAY    // Page 3: Daily Wisdom Quote with Talking Mouth
};

DisplayMode currentMode = MODE_FACE_ANIMATION;

// Function Prototypes
void syncClockWithCompilerTime();
void updateClock();
void updateDayPeriod();
void checkSensors();
void updateXiaozhiFaceLogic();
void renderCurrentMode();
void renderSleepStateMachine();

// Screen Renderers
void renderXiaozhiFaceMode();
void renderAllInOneHUD();
void renderQuoteMode();
void renderArcadeCockpitScene(unsigned long elapsed);

// Graphics Helpers
void drawXiaozhiEyes(int leftH, int rightH, int xShift, int yShift);
void drawXiaozhiMouth(int xShift, int yShift, int w, int h, int radius);
void drawHeartEyes(int xShift, int yShift);
void drawSleepingClosedFace();
void drawMorphingSleepEyes(int curvature, int xShift, int yShift);
void drawMorphingArchEyes(int archCurvature, int xShift, int yShift);
void drawSequentialSleepZ(unsigned long deepSleepElapsed);
void playOldTvPowerOnAnimation();
void playKnobyIntroDialogueAnimation();

// =====================================================================
//  RETRO CRT / OLD TV POWER-ON ANIMATION
// =====================================================================
void playOldTvPowerOnAnimation() {
  // Clear to black
  display.clearDisplay();
  display.display();
  delay(120);

  // Phase 1: Center dot / glowing phosphor pixel blip
  display.clearDisplay();
  display.fillRect(62, 30, 4, 4, SSD1306_WHITE);
  display.display();
  delay(240);

  // Phase 2: Dot rapidly expands horizontally into a thin bright beam (smooth 4px steps)
  for (int w = 4; w <= 128; w += 4) {
    display.clearDisplay();
    int x = (128 - w) / 2;
    display.drawFastHLine(x, 31, w, SSD1306_WHITE);
    display.drawFastHLine(x, 32, w, SSD1306_WHITE);
    display.display();
    delay(2);
  }
  delay(35);

  // Phase 3: Vertical expansion / CRT tube bloom opening up (smooth 2px steps)
  for (int h = 4; h <= 64; h += 2) {
    display.clearDisplay();
    int y = (64 - h) / 2;
    display.fillRect(0, y, 128, h, SSD1306_WHITE);
    display.display();
    delay(2);
  }

  // Phase 4: Full bright phosphor flash & settle
  display.fillScreen(SSD1306_WHITE);
  display.display();
  delay(40);

  display.clearDisplay();
  display.display();
  delay(100);
}

// =====================================================================
//  KNOBY AWAKENING SMILE & SPEAKING INTRO DIALOGUE ("Hi! Im knoby <3")
// =====================================================================
void playKnobyIntroDialogueAnimation() {
  // --- Phase 1: The Awakening Smile (Smooth 20-frame uncurl) ---
  for (int p = 0; p <= 100; p += 5) {
    display.clearDisplay();
    drawMorphingArchEyes(p, 0, 0);
    int mW = 12 + (p * 4 / 100);
    int mH = 2 + (p * 5 / 100);
    drawXiaozhiMouth(0, 0, mW, mH, 3);
    display.display();
    delay(8);
  }
  delay(400); // Hold warm awakening smile

  // --- Phase 2: Dialogue Bubble with Sub-Frame Mouth Tweening ---
  const char dialogueStr[] = "Hi! Im knoby <3";
  const uint8_t dialogueLen = 15;
  int curMouthH = 4;

  // Typewriter effect: 2 smooth tweening frames per character
  for (uint8_t i = 1; i <= dialogueLen; i++) {
    char ch = dialogueStr[i - 1];
    int targetH = (ch == ' ' || ch == '!') ? 2 : (4 + ((i * 3) % 7));

    // 2 intermediate sub-frames per character for silky continuous mouth animation
    for (uint8_t sub = 0; sub < 2; sub++) {
      if (curMouthH < targetH) curMouthH += 2;
      else if (curMouthH > targetH) curMouthH -= 2;

      display.clearDisplay();

      // 1. Speech bubble frame at top
      display.drawRoundRect(10, 2, 108, 16, 3, SSD1306_WHITE);
      // Speech bubble tail pointing towards Knoby's mouth
      display.fillTriangle(60, 17, 68, 17, 64, 21, SSD1306_WHITE);

      // 2. Typewriter text
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(19, 6);
      for (uint8_t j = 0; j < i; j++) {
        display.write(dialogueStr[j]);
      }

      // 3. Cheerful smiling eyes below bubble
      drawMorphingArchEyes(100, 0, 10);

      // 4. Smoothly tweened talking mouth
      drawXiaozhiMouth(0, 6, 14, curMouthH, 2);

      display.display();
      delay(18);
    }
  }

  // --- Phase 3: Completed Dialogue Hold & Resting Smile ---
  delay(1800); // Comfortable hold for reading
}

// =====================================================================
//  SETUP
// =====================================================================
void setup() {
#if ENABLE_SERIAL_DEBUG
  Serial.begin(9600);
  Serial.println(F("========================================"));
  Serial.println(F("              KNOBY v2.0                "));
  Serial.println(F(" Created by Rairolf Rabang & Nicole Sibal"));
  Serial.println(F("========================================"));
#endif

  pinMode(PIN_TILT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_TILT), tiltISR, CHANGE);
  pinMode(PIN_DHT, INPUT_PULLUP);
  pinMode(PIN_LDR, INPUT);

  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
#if ENABLE_SERIAL_DEBUG
    Serial.println(F("ERROR: SSD1306 not found! Check SDA (A4) & SCL (A5)."));
#endif
    for (;;);
  }

  // 400kHz Fast-Mode I2C: cuts buffer transmission from ~100ms down to ~24ms (silky 35+ FPS)
  Wire.setClock(400000);

  display.setRotation(SCREEN_ROTATION);

#if HOLOGRAM_HORIZONTAL_MIRROR
  // SSD1306 Hardware Register: Invert horizontally (X-axis mirror for optical hologram)
  // At Rotation 0: default is 0xA1 (remap); sending 0xA0 inverts horizontally.
  // At Rotation 2: default is 0xA0 (no remap); sending 0xA1 inverts horizontally.
  if (SCREEN_ROTATION == 0) {
    display.ssd1306_command(0xA0);
  } else if (SCREEN_ROTATION == 2) {
    display.ssd1306_command(0xA1);
  }
#endif

  syncClockWithCompilerTime();
  updateDayPeriod();
  updateDHT11();

  // Boot Splash Screen - KNOBY v2.0 & Creators
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(34, 4);
  display.print(F("KNOBY v2.0"));
  display.drawFastHLine(14, 15, 100, SSD1306_WHITE);
  display.setCursor(31, 20);
  display.print(F("Created by:"));
  display.setCursor(22, 31);
  display.print(F("Rairolf Rabang"));
  display.setCursor(61, 42);
  display.print(F("&"));
  display.setCursor(19, 53);
  display.print(F("Margareth Sibal"));
  display.display();
  delay(2200);

  // Retro Old TV (CRT) Power-On Animation (Center pixel -> beam -> full screen)
  playOldTvPowerOnAnimation();

  // Knoby Awakening Smile & Speaking Intro Dialogue ("Hi! Im knoby <3")
  playKnobyIntroDialogueAnimation();

  // Seamlessly transition into active living companion state
  currentSleepState = SLEEP_AWAKE;
  currentMood = MOOD_NORMAL_LIVING;
  currentMouthMode = MOUTH_IDLE;
  currentEyeHeight = BASE_EYE_H;
  currentEyeX = 0;
  currentEyeY = 0;
  targetEyeX = 0;
  targetEyeY = 0;

  lastClockTick = millis();
  lastDHTPollTime = millis();
  nextGazeChangeTime = millis() + 2500;
  nextBlinkTriggerTime = millis() + 3000;
}

// =====================================================================
//  MAIN LOOP
// =====================================================================
void loop() {
  unsigned long frameStart = millis();

  updateClock();
  updateDHT11();
  checkSensors();
  updateXiaozhiFaceLogic();
  renderCurrentMode();

  // Consistent 35 FPS frame governor: ensures zero stutter and zero dropped frames
  unsigned long frameElapsed = millis() - frameStart;
  if (frameElapsed < 28) {
    delay(28 - frameElapsed);
  }
}

// =====================================================================
//  SENSOR & TRIPLE-TAP PROCESSING
// =====================================================================
void checkSensors() {
  unsigned long now = millis();

  // --- 1. Photoresistor (LDR) Ambient Light Tracking ---
  int ldrRaw = analogRead(PIN_LDR);
  if (!isRoomDark && ldrRaw < LDR_DARK_THRESHOLD && now > awakeTemporaryUntil) {
    isRoomDark = true;
    if (currentSleepState == SLEEP_AWAKE) {
      currentSleepState = SLEEP_FALLING_ASLEEP;
      sleepTransitionStartTime = now;
      targetEyeX = 0;
      targetEyeY = 2; // Look slightly down as tiredness sets in
    }
#if ENABLE_SERIAL_DEBUG
    Serial.print(F("[LDR] Lights turned off -> Falling Asleep (ADC: "));
    Serial.print(ldrRaw);
    Serial.println(F(")"));
#endif
  } else if (isRoomDark && ldrRaw > LDR_LIGHT_THRESHOLD) {
    isRoomDark = false;
    if (currentSleepState == SLEEP_DEEP_ASLEEP || currentSleepState == SLEEP_FALLING_ASLEEP) {
      currentSleepState = SLEEP_WAKING_UP;
      sleepTransitionStartTime = now;
    }
#if ENABLE_SERIAL_DEBUG
    Serial.print(F("[LDR] Lights turned on -> Waking up! (ADC: "));
    Serial.print(ldrRaw);
    Serial.println(F(")"));
#endif
  }

#if ENABLE_SERIAL_DEBUG
  // Periodic Serial telemetry every 2 seconds for easy calibration
  static unsigned long lastLDRPrint = 0;
  if (now - lastLDRPrint >= 2000) {
    lastLDRPrint = now;
    Serial.print(F("[LDR Telemetry] Live A0 = "));
    Serial.print(ldrRaw);
    Serial.print(F(" | Sleep State: "));
    if (currentSleepState == SLEEP_AWAKE) Serial.println(F("AWAKE"));
    else if (currentSleepState == SLEEP_FALLING_ASLEEP) Serial.println(F("DROWSY (FALLING ASLEEP)"));
    else if (currentSleepState == SLEEP_DEEP_ASLEEP) Serial.println(F("DEEP ASLEEP"));
    else Serial.println(F("WAKING UP"));
  }
#endif

  // --- 2. Tilt Ball Switch (Curious Single-Tap & Triple-Tap Page Navigation) ---
  bool tapDetected = false;
  noInterrupts();
  if (tiltTriggered) {
    tiltTriggered = false;
    tapDetected = true;
  }
  interrupts();

  int tiltReading = digitalRead(PIN_TILT);
  if (tiltReading != lastTiltState) {
    lastTiltState = tiltReading;
    tapDetected = true;
  }

  if (tapDetected) {
    // Process taps if not locked out by page switch cooldown
    if (now >= tapCooldownUntil) {
      if ((now - lastTapRegisteredTime) > MIN_TAP_INTERVAL) {
        lastTapRegisteredTime = now;

        // Wake buddy smoothly if tapped in dark
        if (isRoomDark) {
          isRoomDark = false;
          awakeTemporaryUntil = now + 12000;
          currentSleepState = SLEEP_WAKING_UP;
          sleepTransitionStartTime = now;
        }

        if (tapCount == 0) {
          tapCount = 1;
          firstTapTime = now;
        } else {
          tapCount++;
          if (tapCount >= 3) {
            // *** TRIPLE TAP: Advance Page! ***
            tapCount = 0;
            isDrivingKart = false;
            currentMode = (DisplayMode)((currentMode + 1) % 3);

            if (currentMode == MODE_QUOTE_DISPLAY) {
              currentQuoteIndex = (currentQuoteIndex + 1) % totalQuotes;
              currentMouthMode = MOUTH_SPEAKING; // Buddy talks while delivering quote!
            } else {
              currentMouthMode = MOUTH_IDLE;
            }

            tapCooldownUntil = now + TRIPLE_TAP_COOLDOWN; // Lockout
#if ENABLE_SERIAL_DEBUG
            Serial.print(F("[TRIPLE TAP] Switched to Page: "));
            Serial.println((int)currentMode);
#endif
          }
        }
      }
    }
  }

  // --- Single Tap Resolver: Trigger 3D First-Person Arcade Cockpit ---
  if (tapCount > 0 && (now - lastTapRegisteredTime >= SINGLE_TAP_RESOLVE_WINDOW)) {
    if (tapCount == 1 && currentMode == MODE_FACE_ANIMATION && currentSleepState == SLEEP_AWAKE) {
      // Confirmed Single Tap! Trigger 3D first-person arcade cockpit racer
      isDrivingKart = true;
      kartStartTime = now;
#if ENABLE_SERIAL_DEBUG
      Serial.println(F("[SINGLE TAP] 3D Arcade Cockpit Activated!"));
#endif
    }
    tapCount = 0; // Clear tap counter after resolution
  }
}

// =====================================================================
//  XIAOZHI FACE ENGINE LOGIC (Saccades, Blinking, Mouth Tweening)
// =====================================================================
void updateXiaozhiFaceLogic() {
  unsigned long now = millis();

  // --- 1. Fluid Gaze Motion ---
  if (currentMode == MODE_FACE_ANIMATION) {
    if (isDrivingKart) {
      // While driving the mini kart, gaze logic stays centered
      targetEyeX = 0;
      targetEyeY = 0;
    } else if (now >= nextGazeChangeTime) {
      // If currently holding an off-center gaze, mostly return to center
      if (abs(targetEyeX) > 2 || abs(targetEyeY) > 2) {
        int roll = random(0, 100);
        if (roll < 75) {
          // Return smoothly to center
          targetEyeX = 0;
          targetEyeY = 0;
          nextGazeChangeTime = now + random(2000, 4500); // Stay center for 2-4.5s
          // Natural blink when returning to center from a wide glance
          if (blinkPhase == 0 && random(0, 100) < 60) {
            blinkPhase = 1;
            lastBlinkPhaseChange = now;
          }
        } else {
          // Double glance: switch from left to right or right to left
          if (targetEyeX < 0) targetEyeX = 13;
          else targetEyeX = -13;
          targetEyeY = random(-2, 3);
          nextGazeChangeTime = now + random(1400, 2400);
        }
      } else {
        // From center, choose an expressive idle gaze direction
        int roll = random(0, 100);
        if (roll < 28) {
          // Glance Left (checking left side of desk)
          targetEyeX = -13;
          targetEyeY = random(-2, 3);
          nextGazeChangeTime = now + random(1500, 3000);
        } else if (roll < 56) {
          // Glance Right (checking right side of desk)
          targetEyeX = 13;
          targetEyeY = random(-2, 3);
          nextGazeChangeTime = now + random(1500, 3000);
        } else if (roll < 72) {
          // Look Up Thoughtfully / Inquisitive
          targetEyeX = random(-3, 4);
          targetEyeY = -7;
          nextGazeChangeTime = now + random(1800, 3200);
        } else if (roll < 84) {
          // Look Diagonal Up-Left or Up-Right
          targetEyeX = (random(0, 2) == 0) ? -10 : 10;
          targetEyeY = -5;
          nextGazeChangeTime = now + random(1400, 2600);
        } else if (roll < 92) {
          // Look Down / Modest Rest
          targetEyeX = random(-3, 4);
          targetEyeY = 5;
          nextGazeChangeTime = now + random(1200, 2200);
        } else {
          // Gentle resting micro-drift
          targetEyeX = random(-3, 4);
          targetEyeY = random(-2, 3);
          nextGazeChangeTime = now + random(2200, 4000);
        }
      }
    }
  }

  // Smooth Glide towards target gaze
  if (currentEyeX < targetEyeX) currentEyeX++;
  else if (currentEyeX > targetEyeX) currentEyeX--;
  if (currentEyeY < targetEyeY) currentEyeY++;
  else if (currentEyeY > targetEyeY) currentEyeY--;

  // --- 2. Fluid Progressive Squash-and-Stretch Blinking ---
  if (blinkPhase == 0) {
    if (now >= nextBlinkTriggerTime) {
      blinkPhase = 1; // Begin closing
      lastBlinkPhaseChange = now;
    }
    currentEyeHeight = BASE_EYE_H;
  } else {
    unsigned long phaseElapsed = now - lastBlinkPhaseChange;
    switch (blinkPhase) {
      case 1: // Smooth squash down (70ms -> ~2.5 frames of smooth closing)
        {
          int bP = (int)(phaseElapsed * (BASE_EYE_H - 2) / 70);
          currentEyeHeight = BASE_EYE_H - bP;
          if (currentEyeHeight < 2) currentEyeHeight = 2;
          if (phaseElapsed >= 70) {
            blinkPhase = 2;
            lastBlinkPhaseChange = now;
            currentEyeHeight = 2;
          }
        }
        break;
      case 2: // Closed slit rest (35ms)
        currentEyeHeight = 2;
        if (phaseElapsed >= 35) {
          blinkPhase = 3;
          lastBlinkPhaseChange = now;
        }
        break;
      case 3: // Smooth stretch & rebound open (80ms -> ~3 frames of smooth opening)
        {
          int bP = (int)(phaseElapsed * (BASE_EYE_H - 2) / 80);
          currentEyeHeight = 2 + bP;
          if (currentEyeHeight > BASE_EYE_H) currentEyeHeight = BASE_EYE_H;
          if (phaseElapsed >= 80) {
            blinkPhase = 0;
            currentEyeHeight = BASE_EYE_H;
            nextBlinkTriggerTime = now + random(2600, 5800); // Natural random blink interval
          }
        }
        break;
    }
  }

  // --- 3. Dynamic Mouth Tweening ---
  if (currentMouthMode == MOUTH_SPEAKING) {
    if (now - lastMouthUpdateTime > 110) {
      lastMouthUpdateTime = now;
      int r = random(0, 100);
      if (r < 25) targetMouthHeight = 3;
      else if (r < 55) targetMouthHeight = 7;
      else if (r < 80) targetMouthHeight = 11;
      else targetMouthHeight = 16;
    }
    // Smooth stepping towards target height
    if (currentMouthHeight < targetMouthHeight) currentMouthHeight += 2;
    else if (currentMouthHeight > targetMouthHeight) currentMouthHeight -= 2;
  } else if (currentMouthMode == MOUTH_LISTENING) {
    currentMouthHeight = 2;
  } else if (currentMouthMode == MOUTH_OPEN_SMILE) {
    currentMouthHeight = 8;
  } else {
    currentMouthHeight = 4; // Resting idle smile bar
  }

  // --- 4. Mood Cycling (While in Page 1) ---
  if (currentMode == MODE_FACE_ANIMATION && !isDrivingKart && currentSleepState == SLEEP_AWAKE) {
    if (now - lastMoodChange >= moodDuration) {
      lastMoodChange = now;
      int roll = random(0, 100);
      if (roll < 30) {
        currentMood = MOOD_NORMAL_LIVING;
        currentMouthMode = MOUTH_IDLE;
        moodDuration = random(3200, 5500);
      } else if (roll < 52) {
        // Questioning / Puzzled with floating '?' emote!
        currentMood = MOOD_QUESTIONING;
        currentMouthMode = MOUTH_LISTENING;
        targetEyeX = 5;
        targetEyeY = -3; // Tilt head and gaze up inquisitively
        moodDuration = random(2600, 4000);
      } else if (roll < 66) {
        // Idea Spark with floating '!' emote!
        currentMood = MOOD_IDEA_SPARK;
        currentMouthMode = MOUTH_OPEN_SMILE;
        targetEyeX = 0;
        targetEyeY = -2;
        moodDuration = random(2200, 3400);
      } else if (roll < 78) {
        currentMood = MOOD_CURIOUS_TILT;
        currentMouthMode = MOUTH_LISTENING;
        moodDuration = random(2000, 3200);
      } else if (roll < 88) {
        currentMood = MOOD_WINK;
        currentMouthMode = MOUTH_IDLE;
        moodDuration = 1400;
      } else if (roll < 95) {
        currentMood = MOOD_EXCITED_ARCHES;
        currentMouthMode = MOUTH_OPEN_SMILE;
        moodDuration = 2200;
      } else {
        currentMood = MOOD_HEART_EYES;
        currentMouthMode = MOUTH_OPEN_SMILE;
        moodDuration = 2500;
      }
    }
  }
}

// =====================================================================
//  RENDER DISPATCHER
// =====================================================================
void renderCurrentMode() {
  display.clearDisplay();

  // Smooth sleep state machine takes priority over standard pages
  if (currentSleepState != SLEEP_AWAKE) {
    renderSleepStateMachine();
    display.display();
    return;
  }

  switch (currentMode) {
    case MODE_FACE_ANIMATION:
      renderXiaozhiFaceMode();
      break;
    case MODE_ALL_IN_ONE_HUD:
      renderAllInOneHUD();
      break;
    case MODE_QUOTE_DISPLAY:
      renderQuoteMode();
      break;
  }

  display.display();
}

// =====================================================================
//  SMOOTH SLEEP & WAKE ENGINE (Yawn, Double Blink, Morph & Sequential Z)
// =====================================================================
void renderSleepStateMachine() {
  unsigned long now = millis();
  unsigned long elapsed = now - sleepTransitionStartTime;

  if (currentSleepState == SLEEP_FALLING_ASLEEP) {
    if (elapsed >= FALLING_ASLEEP_DURATION) {
      currentSleepState = SLEEP_DEEP_ASLEEP;
      drawSleepingClosedFace();
      return;
    }

    // =========================================================
    //  BEDTIME CINEMATIC ROUTINE (3.2 seconds total):
    // =========================================================
    if (elapsed < 1000) {
      // --- ACT 1: THE CUTE YAWN (0.0s - 1.0s) ---
      drawXiaozhiEyes(BASE_EYE_H - 8, BASE_EYE_H - 8, 0, 0);
      int mW, mH;
      if (elapsed < 450) {
        mW = 12 + (6 * (int)elapsed) / 450;
        mH = 4 + (10 * (int)elapsed) / 450;
      } else if (elapsed < 700) {
        mW = 18; mH = 14;
      } else {
        mW = 18 - (6 * (int)(elapsed - 700)) / 300;
        mH = 14 - (11 * (int)(elapsed - 700)) / 300;
      }
      drawXiaozhiMouth(0, 0, mW, mH, 5);
    } 
    else if (elapsed < 1800) {
      // --- ACT 2: DROWSY BLINK #1 (1.0s - 1.8s) ---
      unsigned long bElapsed = elapsed - 1000;
      int eyeH;
      if (bElapsed < 350) {
        eyeH = BASE_EYE_H - (int)(bElapsed * (BASE_EYE_H - 2) / 350);
      } else if (bElapsed < 500) {
        eyeH = 2;
      } else {
        eyeH = 2 + (int)((bElapsed - 500) * 10 / 300);
      }
      drawXiaozhiEyes(eyeH, eyeH, 0, 1);
      drawXiaozhiMouth(0, 0, 14, 3, 2);
    } 
    else if (elapsed < 2400) {
      // --- ACT 3: SLEEPY BLINK #2 (1.8s - 2.4s) ---
      unsigned long b2Elapsed = elapsed - 1800;
      int eyeH = (b2Elapsed < 400) ? (12 - (int)(b2Elapsed * 10 / 400)) : 2;
      drawXiaozhiEyes(eyeH, eyeH, 0, 1);
      drawXiaozhiMouth(0, 0, 12, 2, 2);
    } 
    else {
      // --- ACT 4: CONTINUOUS CRESCENT MORPH (2.4s - 3.2s) ---
      int morphP = (int)((elapsed - 2400) * 100 / 800);
      if (morphP > 100) morphP = 100;
      drawMorphingSleepEyes(morphP, 0, 1);
      drawXiaozhiMouth(0, 0, 12, 2, 2);
    }
  } 
  else if (currentSleepState == SLEEP_DEEP_ASLEEP) {
    // --- ACT 5: DEEP SLEEP & SEQUENTIAL ONE-BY-ONE 'z Z Z' (3.2s+) ---
    drawMorphingSleepEyes(100, 0, 1);
    drawXiaozhiMouth(0, 0, 12, 2, 2);
    unsigned long deepSleepElapsed = elapsed - FALLING_ASLEEP_DURATION;
    drawSequentialSleepZ(deepSleepElapsed);
  } 
  else if (currentSleepState == SLEEP_WAKING_UP) {
    if (elapsed >= WAKING_UP_DURATION) {
      currentSleepState = SLEEP_AWAKE;
      currentMood = MOOD_NORMAL_LIVING;
      currentMouthMode = MOUTH_IDLE;
      currentEyeHeight = BASE_EYE_H;
      currentEyeX = 0;
      currentEyeY = 0;
      targetEyeX = 0;
      targetEyeY = 0;
      lastMoodChange = now;
      moodDuration = random(4500, 6500); // 4.5s - 6.5s guaranteed calm living breathing room!
      return;
    }

    // =========================================================
    //  WAKE-UP CINEMATIC ROUTINE (3.0 seconds total):
    // =========================================================
    if (elapsed < 600) {
      // --- ACT 1: STIRRING & CRESCENT UN-CURL (0.0s - 0.6s) ---
      int unbend = 100 - (int)(elapsed * 100 / 600);
      drawMorphingSleepEyes(unbend, 0, 1);
      drawXiaozhiMouth(0, 0, 12, 2, 2);
    } 
    else if (elapsed < 1400) {
      // --- ACT 2: SLEEPY EYELID FLUTTER & SIGH (0.6s - 1.4s) ---
      unsigned long act2Elapsed = elapsed - 600;
      int eyeH, mouthH;
      if (act2Elapsed < 400) {
        eyeH = 2 + (int)(act2Elapsed * 10 / 400);
        mouthH = 2 + (int)(act2Elapsed * 2 / 400);
      } else {
        eyeH = 12 - (int)((act2Elapsed - 400) * 8 / 400);
        mouthH = 4 - (int)((act2Elapsed - 400) * 2 / 400);
      }
      drawXiaozhiEyes(eyeH, eyeH, 0, 1);
      drawXiaozhiMouth(0, 0, 13, mouthH, 2);
    } 
    else if (elapsed < 1950) {
      // --- ACT 3: WIDE-EYE WAKE STRETCH & MORNING SHINE (1.4s - 1.95s) ---
      unsigned long act3Elapsed = elapsed - 1400;
      int eyeH = (act3Elapsed < 412) ? (4 + (int)(act3Elapsed * 26 / 412)) : (30 - (int)((act3Elapsed - 412) * 2 / 138));
      int mouthH = 2 + (int)(act3Elapsed * 4 / 550);
      int mouthW = 13 + (int)(act3Elapsed * 3 / 550);
      drawXiaozhiEyes(eyeH, eyeH, 0, 0);
      drawXiaozhiMouth(0, 0, mouthW, mouthH, 3);
    } 
    else if (elapsed < 2250) {
      // --- ACT 4A: FLUID SMILE SQUASH INTO CHEERFUL ARCH (1.95s - 2.25s) ---
      unsigned long aElapsed = elapsed - 1950;
      if (aElapsed < 170) {
        int eyeH = 28 - (int)(aElapsed * 25 / 170);
        int mW = 16 + (int)(aElapsed * 2 / 170);
        int mH = 6 + (int)(aElapsed * 2 / 170);
        drawXiaozhiEyes(eyeH, eyeH, 0, 0);
        drawXiaozhiMouth(0, 0, mW, mH, 4);
      } else {
        int pCurv = (int)((aElapsed - 170) * 100 / 80);
        if (pCurv > 100) pCurv = 100;
        drawMorphingArchEyes(pCurv, 0, 0);
        drawXiaozhiMouth(0, 0, 18, 8, 4);
      }
    }
    else if (elapsed < 2650) {
      // --- ACT 4B: JOYFUL GIGGLE & MORNING BOUNCE (2.25s - 2.65s) ---
      unsigned long bElapsed = elapsed - 2250;
      long bCycle = (long)(bElapsed % 200);
      int bounceY = (int)((8L * bCycle * (200L - bCycle)) / 10000L);
      drawMorphingArchEyes(100, 0, -bounceY);
      drawXiaozhiMouth(0, -bounceY, 18, 8, 4);
    }
    else {
      // --- ACT 4C: FLUID MORPH-OUT BACK TO AWAKE GAZE (2.65s - 3.0s) ---
      unsigned long outElapsed = elapsed - 2650;
      if (outElapsed < 120) {
        int unArch = 100 - (int)(outElapsed * 100 / 120);
        drawMorphingArchEyes(unArch, 0, 0);
        drawXiaozhiMouth(0, 0, 18, 7, 4);
      } else {
        int pBloom = (int)(outElapsed - 120);
        int eyeH = 3 + pBloom * 25 / 230;
        int mW = 18 - pBloom * 2 / 230;
        int mH = 7 - pBloom * 4 / 230;
        drawXiaozhiEyes(eyeH, eyeH, 0, 0);
        drawXiaozhiMouth(0, 0, mW, mH, 3);
      }
    }
  }
}

// =====================================================================
//  STYLE B: 3D FIRST-PERSON ARCADE COCKPIT RACER (Single-Tap Reaction)
// =====================================================================
void renderArcadeCockpitScene(unsigned long elapsed) {
  // If total duration reached, return to living face
  if (elapsed >= KART_TOTAL_DURATION) {
    isDrivingKart = false;
    currentMood = MOOD_NORMAL_LIVING;
    lastMoodChange = millis();
    moodDuration = random(3000, 5000);
    currentMouthMode = MOUTH_IDLE;
    return;
  }

  // ===================================================================
  //  PHASE 4: CINEMATIC VICTORY WINK & TWINKLING SPARKLE STAR (1800 - 2600ms)
  // ===================================================================
  if (elapsed >= 1800) {
    // Left eye open & glossy, right eye slit
    drawXiaozhiEyes(BASE_EYE_H, 2, 0, 0);
    // Anime wink arch over right eye
    display.drawLine(81, 29, 92, 24, SSD1306_WHITE);
    display.drawLine(92, 24, 103, 29, SSD1306_WHITE);

    // Warm champion smile
    drawXiaozhiMouth(0, 0, 18, 6, 3);

    // Twinkling 4-point anime sparkle star on the side
    int sLen = 3 + (((elapsed / 70) % 2 == 0) ? 2 : 0);
    display.drawFastVLine(114, 18 - sLen, sLen * 2 + 1, SSD1306_WHITE);
    display.drawFastHLine(114 - sLen, 18, sLen * 2 + 1, SSD1306_WHITE);
    display.drawRect(113, 17, 3, 3, SSD1306_WHITE);
    display.drawPixel(109, 13, SSD1306_WHITE);
    display.drawPixel(119, 23, SSD1306_WHITE);
    return;
  }

  // ===================================================================
  //  PHASES 1, 2, 3: 3D ARCADE COCKPIT & CAMERA PAN-OUT (0 - 1800ms)
  // ===================================================================

  // 1. Organic Buttery S-Curve Road Swerve (0 - 1150ms) using 16-bit ease
  int curveOffset = 0;
  if (elapsed >= 350 && elapsed < 1150) {
    int t = (int)(elapsed - 350); // 0 to 800ms
    if (t < 400) {
      curveOffset = -(int)(((unsigned int)t * (400 - t)) / 625);
    } else {
      int t2 = t - 400;
      curveOffset = (int)(((unsigned int)t2 * (400 - t2)) / 625);
    }
  }

  const int horizonY = 27;
  int vpX = 64 + curveOffset;

  // 2. Camera Pan-Out Slide Down (1500 - 1800ms)
  int panDownY = (elapsed >= 1500) ? (int)((elapsed - 1500) / 10) : 0;

  // 3. Horizon Mountains with Parallax
  int mShift = curveOffset / 3;
  for (byte m = 0; m < 2; m++) {
    int mx = (m == 0) ? (10 - mShift) : (86 - mShift);
    display.drawTriangle(mx, horizonY, mx + 16, 18 + m * 2, mx + 32, horizonY, SSD1306_WHITE);
  }
  display.drawFastHLine(0, horizonY, 128, SSD1306_WHITE);

  // 4. 3D Perspective Road
  int dashTopY = 48 + panDownY;
  display.drawLine(vpX - 3, horizonY, 12 + (curveOffset / 2), dashTopY, SSD1306_WHITE);
  display.drawLine(vpX + 3, horizonY, 116 + (curveOffset / 2), dashTopY, SSD1306_WHITE);

  // 5. Exponential 3D Center Dashes (16-bit integer math)
  int scrollSpeed = (elapsed >= 1150) ? 11 : 16;
  for (byte d = 0; d < 3; d++) {
    int prog = (int)(((elapsed / scrollSpeed) + d * 28) % 84);
    int dy = horizonY + (prog * prog) / 336;
    int dLen = 1 + (prog * 6) / 84;
    int dx = vpX + ((64 - vpX) * (dy - horizonY)) / (48 - horizonY);
    if (dy + dLen < dashTopY) {
      display.drawFastVLine(dx, dy, dLen, SSD1306_WHITE);
    }
  }

  // 6. Roadside Rumble Markers
  for (byte r = 0; r < 2; r++) {
    int prog = (int)(((elapsed / scrollSpeed) + r * 42) % 84);
    int ry = horizonY + (prog * prog) / 336;
    int spread = 4 + (prog * 50) / 84;
    if (ry < dashTopY) {
      display.drawFastHLine(vpX - spread - 3, ry, 3, SSD1306_WHITE);
      display.drawFastHLine(vpX + spread, ry, 3, SSD1306_WHITE);
    }
  }

  // 7. Rearview Mirror with Knoby's Eyes
  if (panDownY < 12) {
    display.drawRoundRect(48, 1 - panDownY, 32, 9, 2, SSD1306_WHITE);
    display.drawFastVLine(64, 0, 2, SSD1306_WHITE);
    display.drawLine(55, 3 - panDownY, 58, 5 - panDownY, SSD1306_WHITE);
    display.drawLine(58, 5 - panDownY, 55, 7 - panDownY, SSD1306_WHITE);
    display.drawLine(69, 3 - panDownY, 72, 5 - panDownY, SSD1306_WHITE);
    display.drawLine(72, 5 - panDownY, 69, 7 - panDownY, SSD1306_WHITE);
  }

  // 8. HUD Speedometer & Flag
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  if (elapsed >= 1150 && elapsed < 1500) {
    display.print(F("NITRO!"));
  } else if (elapsed < 1150) {
    int spd = (elapsed < 350) ? (int)(elapsed * 160L / 350L) : (160 + (curveOffset * 2));
    display.print(spd);
    display.print(F("K"));
  }

  display.drawFastVLine(124, 2, 11, SSD1306_WHITE);
  int fWave = ((elapsed / 90) % 2 == 0) ? 0 : 1;
  display.drawRect(116, 2 + fWave, 8, 5, SSD1306_WHITE);
  display.fillRect(116, 2 + fWave, 4, 3, SSD1306_WHITE);
  display.fillRect(120, 4 + fWave, 4, 3, SSD1306_WHITE);

  // 9. Nitro Warp Lines
  if (elapsed >= 1150 && elapsed < 1500) {
    for (byte w = 0; w < 4; w++) {
      int wAngle = (int)(((elapsed * 3) + w * 45) % 180);
      int x1 = (wAngle < 90) ? 0 : 127;
      int y1 = (wAngle % 50);
      display.drawLine(x1, y1, vpX, horizonY, SSD1306_WHITE);
    }
  }

  // 10. Dashboard & Steer Wheel (Slides down during pan-out)
  if (dashTopY < 64) {
    int dashShake = (elapsed >= 1150 && elapsed < 1500) ? (((elapsed / 30) % 2 == 0) ? 1 : 0) : (((elapsed / 60) % 2 == 0) ? 1 : 0);
    int dY = dashTopY + dashShake;
    display.drawFastHLine(0, dY, 128, SSD1306_WHITE);
    display.drawFastHLine(0, dY + 1, 128, SSD1306_WHITE);

    display.drawCircle(20, dY + 8, 5, SSD1306_WHITE);
    display.drawLine(20, dY + 8, 23, dY + 6, SSD1306_WHITE);

    display.drawRect(98, dY + 5, 20, 5, SSD1306_WHITE);
    int bars = (elapsed >= 1150) ? 16 : (4 + ((elapsed / 70) % 12));
    display.fillRect(100, dY + 6, bars, 3, SSD1306_WHITE);

    int steerTilt = (curveOffset * 3) / 5;
    int sx = 64 + steerTilt / 2;
    int sy = dY + 8;

    display.drawLine(sx - 14, sy - 5 - steerTilt/2, sx + 14, sy - 5 + steerTilt/2, SSD1306_WHITE);
    display.drawLine(sx - 12, sy + 5 - steerTilt/2, sx + 12, sy + 5 + steerTilt/2, SSD1306_WHITE);
    display.drawFastVLine(sx - 14, sy - 5 - steerTilt/2, 10, SSD1306_WHITE);
    display.drawFastVLine(sx + 14, sy - 5 + steerTilt/2, 10, SSD1306_WHITE);
    display.fillCircle(sx, sy, 2, SSD1306_WHITE);

    for (int s = -1; s <= 1; s += 2) {
      int px = sx + s * 14;
      int py = sy + (s * steerTilt) / 2;
      display.fillCircle(px, py, 3, SSD1306_WHITE);
      display.drawPixel(px, py, SSD1306_BLACK);
    }
  }
}

// =====================================================================
//  PAGE 1: XIAOZHI COMPANION FACE (Dynamic Eyes + Mouth)
// =====================================================================
void renderXiaozhiFaceMode() {
  unsigned long now = millis();

  // If 3D Arcade Cockpit reaction is active, render first-person cockpit scene!
  if (isDrivingKart) {
    renderArcadeCockpitScene(now - kartStartTime);
    return;
  }

  // Note: Sleep is controlled by room lighting (Photoresistor LDR).
  // If your room lights are on, Desk Buddy stays awake as your late-night companion!

  // Render Based on Companion Mood (Pure Full-Screen Face, No Cluttering Text)
  int effX = (int)currentEyeX;
  int effY = (int)currentEyeY;

  unsigned long mElapsed = millis() - lastMoodChange;
  unsigned long mRemaining = (moodDuration > mElapsed) ? (moodDuration - mElapsed) : 0;

  // Smooth Envelope for Moods (220ms intro, 220ms outro)
  int easeWeight = 100;
  if (mElapsed < 220) {
    easeWeight = (int)(mElapsed * 100 / 220);
  } else if (mRemaining < 220) {
    easeWeight = (int)(mRemaining * 100 / 220);
  }

  switch (currentMood) {
    case MOOD_NORMAL_LIVING:
      drawXiaozhiEyes(currentEyeHeight, currentEyeHeight, effX, effY);
      drawXiaozhiMouth(effX * 3 / 5, effY * 3 / 5, 18, currentMouthHeight, 3);
      break;

    case MOOD_QUESTIONING:
      {
        // Smoothly squints right eye (28px -> 18px -> 28px)
        int rightH = BASE_EYE_H - (easeWeight * 10 / 100);
        drawXiaozhiEyes(BASE_EYE_H, rightH, effX, effY);

        // Cocked inquisitive smirk smoothly interpolates
        int mW = 18 - (easeWeight * 6 / 100);
        int mXOff = -(easeWeight * 4 / 100);
        drawXiaozhiMouth(effX * 3 / 5 + mXOff, effY * 3 / 5, mW, 3, 2);

        // Animated floating Question Mark '?'
        int hoverBob = (millis() / 170) % 3; // Gentle 2px hover bob
        int baseY = 2 + hoverBob;
        int qY;
        if (mElapsed < 220) {
          qY = baseY + (int)((220 - mElapsed) * 14 / 220);
        } else if (mRemaining < 220) {
          qY = baseY - (int)((220 - mRemaining) * 18 / 220);
        } else {
          qY = baseY;
        }

        if (qY > -14 && qY < 40) {
          display.setTextSize(2);
          display.setCursor(104, qY);
          display.print(F("?"));
        }
      }
      break;

    case MOOD_IDEA_SPARK:
      {
        drawXiaozhiEyes(BASE_EYE_H, BASE_EYE_H, effX, effY);

        // Open cheerful smile smoothly blooms open and relaxes
        int mW = 18 - (easeWeight * 2 / 100);
        int mH = currentMouthHeight + (easeWeight * 3 / 100);
        drawXiaozhiMouth(effX * 3 / 5, effY * 3 / 5 - (easeWeight * 2 / 100), mW, mH, 3);

        // Animated floating '!' with sparkle rays
        int hoverBob = (millis() / 170) % 2;
        int baseY = 2 + hoverBob;
        int sparkY;
        int disperse = 0;
        if (mElapsed < 220) {
          sparkY = baseY + (int)((220 - mElapsed) * 14 / 220);
        } else if (mRemaining < 220) {
          sparkY = baseY - (int)((220 - mRemaining) * 18 / 220);
          disperse = (int)((220 - mRemaining) * 6 / 220);
        } else {
          sparkY = baseY;
        }

        if (sparkY > -14 && sparkY < 40) {
          display.setTextSize(2);
          display.setCursor(106, sparkY);
          display.print(F("!"));

          // Sparkles disperse outward on exit
          display.drawPixel(100 - disperse, sparkY + 2, SSD1306_WHITE);
          display.drawPixel(118 + disperse, sparkY + 2, SSD1306_WHITE);
          display.drawPixel(119 + disperse, sparkY + 1, SSD1306_WHITE);
        }
      }
      break;

    case MOOD_CURIOUS_TILT:
      {
        int leftH = BASE_EYE_H - (easeWeight * 5 / 100);
        drawXiaozhiEyes(leftH, BASE_EYE_H, effX, effY);
        int mW = 18 - (easeWeight * 6 / 100);
        int mXOff = -(easeWeight * 4 / 100);
        drawXiaozhiMouth(effX * 3 / 5 + mXOff, effY * 3 / 5, mW, 3, 2);
      }
      break;

    case MOOD_WINK:
      {
        // Left eye open
        display.fillRoundRect(LEFT_EYE_CX - BASE_EYE_W/2 + effX, BASE_EYE_CY - BASE_EYE_H/2 + effY, BASE_EYE_W, BASE_EYE_H, EYE_RADIUS, SSD1306_WHITE);
        display.fillCircle(LEFT_EYE_CX - 3 + effX, BASE_EYE_CY - 4 + effY, 3, SSD1306_BLACK);
        display.drawPixel(LEFT_EYE_CX - 2 + effX, BASE_EYE_CY - 5 + effY, SSD1306_WHITE);

        // Right eye smooth wink transition
        if (mElapsed < 120) {
          int rH = BASE_EYE_H - (int)(mElapsed * (BASE_EYE_H - 2) / 120);
          int rad = (rH / 2 < EYE_RADIUS) ? (rH / 2) : EYE_RADIUS;
          display.fillRoundRect(RIGHT_EYE_CX - BASE_EYE_W/2 + effX, BASE_EYE_CY - rH/2 + effY, BASE_EYE_W, rH, rad, SSD1306_WHITE);
        } else if (mRemaining < 120) {
          int rH = 2 + (int)((120 - mRemaining) * (BASE_EYE_H - 2) / 120);
          int rad = (rH / 2 < EYE_RADIUS) ? (rH / 2) : EYE_RADIUS;
          display.fillRoundRect(RIGHT_EYE_CX - BASE_EYE_W/2 + effX, BASE_EYE_CY - rH/2 + effY, BASE_EYE_W, rH, rad, SSD1306_WHITE);
        } else {
          // Wink arc lines
          display.drawLine(RIGHT_EYE_CX - 12 + effX, BASE_EYE_CY + 4 + effY, RIGHT_EYE_CX + effX, BASE_EYE_CY - 4 + effY, SSD1306_WHITE);
          display.drawLine(RIGHT_EYE_CX + effX, BASE_EYE_CY - 4 + effY, RIGHT_EYE_CX + 12 + effX, BASE_EYE_CY + 4 + effY, SSD1306_WHITE);
          display.drawLine(RIGHT_EYE_CX - 12 + effX, BASE_EYE_CY + 5 + effY, RIGHT_EYE_CX + effX, BASE_EYE_CY - 3 + effY, SSD1306_WHITE);
          display.drawLine(RIGHT_EYE_CX + effX, BASE_EYE_CY - 3 + effY, RIGHT_EYE_CX + 12 + effX, BASE_EYE_CY + 5 + effY, SSD1306_WHITE);
        }
        drawXiaozhiMouth(effX * 3 / 5, effY * 3 / 5, 16, 4, 3);
      }
      break;

    case MOOD_HEART_EYES:
      {
        if (mElapsed < 130) {
          int eyeH = BASE_EYE_H - (int)(mElapsed * (BASE_EYE_H - 4) / 130);
          drawXiaozhiEyes(eyeH, eyeH, effX, effY);
        } else if (mRemaining < 130) {
          int eyeH = 4 + (int)((130 - mRemaining) * (BASE_EYE_H - 4) / 130);
          drawXiaozhiEyes(eyeH, eyeH, effX, effY);
        } else {
          drawHeartEyes(effX, effY);
        }
        int mW = 16 + (easeWeight * 2 / 100);
        int mH = 4 + (easeWeight * 4 / 100);
        drawXiaozhiMouth(effX * 3 / 5, effY * 3 / 5, mW, mH, 4);
      }
      break;

    case MOOD_EXCITED_ARCHES:
      {
        if (mElapsed < 120) {
          int eyeH = BASE_EYE_H - (int)(mElapsed * (BASE_EYE_H - 3) / 120);
          drawXiaozhiEyes(eyeH, eyeH, effX, effY);
        } else if (mElapsed < 200) {
          int p = (int)((mElapsed - 120) * 100 / 80);
          drawMorphingArchEyes(p, effX, effY);
        } else if (mRemaining < 120) {
          int eyeH = 3 + (int)((120 - mRemaining) * (BASE_EYE_H - 3) / 120);
          drawXiaozhiEyes(eyeH, eyeH, effX, effY);
        } else if (mRemaining < 200) {
          int p = (int)((mRemaining - 120) * 100 / 80);
          drawMorphingArchEyes(p, effX, effY);
        } else {
          int bobY = ((millis() / 180) % 2 == 0) ? -1 : 0;
          drawMorphingArchEyes(100, effX, effY + bobY);
        }
        int mW = 18 + (easeWeight * 2 / 100);
        int mH = 4 + (easeWeight * 5 / 100);
        drawXiaozhiMouth(effX * 3 / 5, effY * 3 / 5, mW, mH, 4);
      }
      break;
  }
}

// ---------------------------------------------------------------------
//  XIAOZHI GRAPHICS PRIMITIVES
// ---------------------------------------------------------------------
void drawXiaozhiEyes(int leftH, int rightH, int xShift, int yShift) {
  int cy = BASE_EYE_CY + yShift;

  // Left Eye Capsule
  int leftTopY = cy - leftH / 2;
  display.fillRoundRect(LEFT_EYE_CX - BASE_EYE_W/2 + xShift, leftTopY, BASE_EYE_W, leftH, EYE_RADIUS, SSD1306_WHITE);

  // Right Eye Capsule
  int rightTopY = cy - rightH / 2;
  display.fillRoundRect(RIGHT_EYE_CX - BASE_EYE_W/2 + xShift, rightTopY, BASE_EYE_W, rightH, EYE_RADIUS, SSD1306_WHITE);

  // Sparkle Pupils with parallax anticipation (Only visible when eyes are well open)
  if (leftH > 12) {
    int pupilOffX = (xShift * 2) / 7; // Subtly leads the gaze inside the eye
    int pupilOffY = (yShift * 2) / 7;
    display.fillCircle(LEFT_EYE_CX - 3 + xShift + pupilOffX, cy - 4 + pupilOffY, 3, SSD1306_BLACK);
    display.drawPixel(LEFT_EYE_CX - 2 + xShift + pupilOffX, cy - 5 + pupilOffY, SSD1306_WHITE);
  }
  if (rightH > 12) {
    int pupilOffX = (xShift * 2) / 7;
    int pupilOffY = (yShift * 2) / 7;
    display.fillCircle(RIGHT_EYE_CX - 3 + xShift + pupilOffX, cy - 4 + pupilOffY, 3, SSD1306_BLACK);
    display.drawPixel(RIGHT_EYE_CX - 2 + xShift + pupilOffX, cy - 5 + pupilOffY, SSD1306_WHITE);
  }
}

void drawXiaozhiMouth(int xShift, int yShift, int w, int h, int radius) {
  int mx = 64 - w / 2 + xShift;
  int my = 48 + yShift;
  display.fillRoundRect(mx, my, w, h, radius, SSD1306_WHITE);
}

void drawHeartEyes(int xShift, int yShift) {
  int centers[2] = {LEFT_EYE_CX + xShift, RIGHT_EYE_CX + xShift};
  int cy = BASE_EYE_CY - 2 + yShift;
  for (int i = 0; i < 2; i++) {
    int cx = centers[i];
    display.fillCircle(cx - 5, cy - 3, 6, SSD1306_WHITE);
    display.fillCircle(cx + 5, cy - 3, 6, SSD1306_WHITE);
    display.fillTriangle(cx - 11, cy, cx + 11, cy, cx, cy + 12, SSD1306_WHITE);
  }
}



void drawMorphingSleepEyes(int curvature, int xShift, int yShift) {
  int centers[2] = {LEFT_EYE_CX + xShift, RIGHT_EYE_CX + xShift};
  for (int e = 0; e < 2; e++) {
    int cx = centers[e];
    for (int dx = -9; dx <= 9; dx++) {
      int sag = (curvature * (81 - dx * dx) * 9) / (81 * 200);
      int py = BASE_EYE_CY + yShift + sag;
      display.drawPixel(cx + dx, py, SSD1306_WHITE);
      display.drawPixel(cx + dx, py + 1, SSD1306_WHITE);
      if (curvature > 40) {
        display.drawPixel(cx + dx, py + 2, SSD1306_WHITE);
      }
    }
  }
}

void drawMorphingArchEyes(int archCurvature, int xShift, int yShift) {
  int centers[2] = {LEFT_EYE_CX + xShift, RIGHT_EYE_CX + xShift};
  for (int e = 0; e < 2; e++) {
    int cx = centers[e];
    for (int dx = -9; dx <= 9; dx++) {
      int rise = (archCurvature * (81 - dx * dx) * 5) / (81 * 100);
      int py = BASE_EYE_CY + yShift - rise;
      display.drawPixel(cx + dx, py, SSD1306_WHITE);
      display.drawPixel(cx + dx, py + 1, SSD1306_WHITE);
      if (archCurvature > 35) {
        display.drawPixel(cx + dx, py + 2, SSD1306_WHITE);
      }
    }
  }
}

void drawSequentialSleepZ(unsigned long deepSleepElapsed) {
  // Before 500ms of deep sleep: no letters (peaceful silent sleep)
  if (deepSleepElapsed < 500) return;

  // Staggered breathing loop period = 2400ms
  unsigned long cycle = 2400;

  // 1. First tiny 'z' (spawns at +500ms)
  unsigned long t1 = (deepSleepElapsed - 500) % cycle;
  int z1Y = 24 - (int)((t1 * 12) / cycle); // drifts from Y=24 up to Y=12
  int z1X = 96 + (int)((t1 * 6) / cycle);  // drifts slightly right
  display.setTextSize(1);
  display.setCursor(z1X, z1Y);
  display.print(F("z"));

  // 2. Second medium 'z' (spawns 750ms later, at +1250ms)
  if (deepSleepElapsed >= 1250) {
    unsigned long t2 = (deepSleepElapsed - 1250) % cycle;
    int z2Y = 18 - (int)((t2 * 12) / cycle); // drifts from Y=18 up to Y=6
    int z2X = 104 + (int)((t2 * 6) / cycle);
    display.setTextSize(1);
    display.setCursor(z2X, z2Y);
    display.print(F("z"));
  }

  // 3. Third big bold 'Z' (spawns another 750ms later, at +2000ms)
  if (deepSleepElapsed >= 2000) {
    unsigned long t3 = (deepSleepElapsed - 2000) % cycle;
    int z3Y = 12 - (int)((t3 * 11) / cycle); // drifts from Y=12 up to Y=1
    int z3X = 112 + (int)((t3 * 5) / cycle);
    display.setTextSize(2);
    display.setCursor(z3X, z3Y);
    display.print(F("Z"));
  }
}

void drawSleepingClosedFace() {
  drawMorphingSleepEyes(100, 0, 1);
  drawXiaozhiMouth(0, 0, 12, 2, 2);
}

// =====================================================================
//  PAGE 2: ALL-IN-ONE DASHBOARD HUD (Clock, Date, Temp, Humidity, Light)
// =====================================================================
void renderAllInOneHUD() {
  int tempC = lastDHTReading.temperatureC;
  int hum   = lastDHTReading.humidity;

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // --- 1. Top Status Pills ---
  // Left: Date Badge Pill [OCT 13]
  display.drawRoundRect(2, 1, 54, 11, 2, SSD1306_WHITE);
  display.setCursor(6, 3);
  const char monthNames[] PROGMEM = "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC";
  int mIdx = (currentMonth >= 1 && currentMonth <= 12) ? (currentMonth - 1) * 3 : 0;
  for (uint8_t k = 0; k < 3; k++) {
    display.write((char)pgm_read_byte(&monthNames[mIdx + k]));
  }
  display.print(' ');
  if (currentDay < 10) display.print('0');
  display.print(currentDay);

  // Right: Comfort & Ambient Light Pill [COMFY DAY]
  display.drawRoundRect(60, 1, 66, 11, 2, SSD1306_WHITE);
  display.setCursor(65, 3);
  if (hum < 35) display.print(F("DRY"));
  else if (hum > 68) display.print(F("HUMID"));
  else display.print(F("COMFY"));

  display.setCursor(102, 3);
  int ldrRaw = analogRead(PIN_LDR);
  if (ldrRaw < LDR_DARK_THRESHOLD) display.print(F("NTE"));
  else display.print(F("DAY"));

  // --- 2. Left Hero Clock Card ---
  display.drawRoundRect(1, 14, 64, 36, 3, SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(3, 18);
  if (currentHours < 10) display.print('0');
  display.print(currentHours);
  display.print((millis() / 500) % 2 ? ':' : ' ');
  if (currentMinutes < 10) display.print('0');
  display.print(currentMinutes);

  // Subscript Seconds & AM/PM
  display.setTextSize(1);
  display.setCursor(5, 38);
  display.print(F("S:"));
  if (currentSeconds < 10) display.print('0');
  display.print(currentSeconds);
  display.setCursor(40, 38);
  display.print(currentHours < 12 ? F("AM") : F("PM"));

  // --- 3. Right Dual Sensor Gauge Cards ---
  // Top: Temperature Widget
  display.drawRoundRect(67, 14, 59, 17, 2, SSD1306_WHITE);
  // Pixel Thermometer Glyph
  display.drawFastVLine(71, 16, 5, SSD1306_WHITE);
  display.fillCircle(71, 21, 2, SSD1306_WHITE);
  display.setCursor(77, 16);
  display.print(tempC);
  display.write((char)247);
  display.print('C');
  // Visual Temp Level Bar
  display.drawRect(71, 26, 51, 3, SSD1306_WHITE);
  int tBar = (tempC - 15) * 5 / 2;
  if (tBar < 2) tBar = 2;
  else if (tBar > 49) tBar = 49;
  display.fillRect(72, 27, tBar, 1, SSD1306_WHITE);

  // Bottom: Humidity Widget
  display.drawRoundRect(67, 33, 59, 17, 2, SSD1306_WHITE);
  // Pixel Water Droplet Glyph
  display.drawPixel(71, 35, SSD1306_WHITE);
  display.drawLine(70, 37, 72, 37, SSD1306_WHITE);
  display.fillCircle(71, 39, 2, SSD1306_WHITE);
  display.setCursor(77, 35);
  display.print(hum);
  display.print('%');
  // Visual Humidity Level Bar
  display.drawRect(71, 45, 51, 3, SSD1306_WHITE);
  int hBar = (hum - 20) * 7 / 10;
  if (hBar < 2) hBar = 2;
  else if (hBar > 49) hBar = 49;
  display.fillRect(72, 46, hBar, 1, SSD1306_WHITE);

  // --- 4. Bottom Navigation & Status Bar ---
  display.setCursor(4, 54);
  if (millis() < tapCooldownUntil) {
    display.print(F("[LOCKED]"));
  } else {
    display.print(F("KNOBY HUD"));
  }

  // Symmetrical 3-Dot Page Indicator [ o  (o)  o ]
  display.drawCircle(102, 57, 2, SSD1306_WHITE); // Page 1
  display.fillCircle(112, 57, 3, SSD1306_WHITE); // Page 2 (active hero)
  display.drawCircle(122, 57, 2, SSD1306_WHITE); // Page 3
}


// =====================================================================
//  PAGE 3: DAILY WISDOM QUOTE WITH SMARTWATCH CARD & LIVE AUDIO WAVES
// =====================================================================
void renderQuoteMode() {
  // --- 1. Top Badges Row (Symmetrical) ---
  // Left: Daily Wisdom Pill
  display.drawRoundRect(2, 1, 74, 11, 2, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(6, 3);
  display.print(F("DAILY WISDOM"));

  // Right: Quote Counter Badge [01/03]
  display.drawRoundRect(80, 1, 46, 11, 2, SSD1306_WHITE);
  display.setCursor(86, 3);
  display.print('0');
  display.print(currentQuoteIndex + 1);
  display.print(F("/03"));

  // --- 2. Hero Rounded Speech Card ---
  display.drawRoundRect(1, 14, 126, 38, 3, SSD1306_WHITE);
  display.setCursor(7, 18);
  display.write('"');

  PGM_P qPtr = (PGM_P)pgm_read_word(&(quotesTable[currentQuoteIndex]));
  uint8_t lineLen = 1;
  uint8_t lineY = 18;
  for (uint8_t i = 0;; i++) {
    char c = (char)pgm_read_byte(qPtr + i);
    if (c == '\0') {
      display.write('"');
      break;
    }
    if (lineLen >= 17 && c == ' ') {
      lineY += 9;
      display.setCursor(7, lineY);
      lineLen = 0;
      continue;
    }
    display.write(c);
    lineLen++;
  }

  // --- 3. Speaker Badge & Live Voice Audio Waves ---
  display.setCursor(8, 40);
  display.print(F("- Knoby"));

  // Live chattering mouth (centered at x=82, y=43)
  drawXiaozhiMouth(18, -5, 14, currentMouthHeight, 2);

  // Dynamic voice waves when mouth is speaking
  if (currentMouthHeight > 2) {
    display.drawPixel(94, 43, SSD1306_WHITE);
    display.drawFastVLine(97, 41, 5, SSD1306_WHITE);
    display.drawFastVLine(100, 39, 9, SSD1306_WHITE);
  }

  // --- 4. Bottom Navigation & Status Bar ---
  display.setCursor(4, 54);
  if (millis() < tapCooldownUntil) {
    display.print(F("[LOCKED]"));
  } else {
    display.print(F("KNOBY TALKS"));
  }

  // Symmetrical 3-Dot Page Indicator [ o  o  (o) ] (Page 3 active!)
  display.drawCircle(102, 57, 2, SSD1306_WHITE); // Page 1
  display.drawCircle(112, 57, 2, SSD1306_WHITE); // Page 2
  display.fillCircle(122, 57, 3, SSD1306_WHITE); // Page 3 (active hero)
}

// =====================================================================
//  AUTOMATIC COMPILER REAL-TIME SYNCHRONIZATION
// =====================================================================
void syncClockWithCompilerTime() {
  const char* timeStr = __TIME__;
  currentHours   = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
  currentMinutes = (timeStr[3] - '0') * 10 + (timeStr[4] - '0');
  currentSeconds = (timeStr[6] - '0') * 10 + (timeStr[7] - '0');

  const char* dateStr = __DATE__;
  currentYear = (dateStr[7] - '0') * 1000 + (dateStr[8] - '0') * 100 + (dateStr[9] - '0') * 10 + (dateStr[10] - '0');
  currentDay  = (dateStr[4] == ' ' ? 0 : (dateStr[4] - '0') * 10) + (dateStr[5] - '0');

  char m0 = dateStr[0];
  char m1 = dateStr[1];
  char m2 = dateStr[2];
  if (m0 == 'J') {
    if (m1 == 'a') currentMonth = 1;
    else if (m2 == 'n') currentMonth = 6;
    else currentMonth = 7;
  } else if (m0 == 'F') currentMonth = 2;
  else if (m0 == 'M') currentMonth = (m2 == 'r') ? 3 : 5;
  else if (m0 == 'A') currentMonth = (m1 == 'p') ? 4 : 8;
  else if (m0 == 'S') currentMonth = 9;
  else if (m0 == 'O') currentMonth = 10;
  else if (m0 == 'N') currentMonth = 11;
  else if (m0 == 'D') currentMonth = 12;

#if ENABLE_SERIAL_DEBUG
  Serial.print(F("Clock Synced to: "));
  if (currentHours < 10) Serial.print('0');
  Serial.print(currentHours);
  Serial.print(':');
  if (currentMinutes < 10) Serial.print('0');
  Serial.print(currentMinutes);
  Serial.print(':');
  if (currentSeconds < 10) Serial.print('0');
  Serial.println(currentSeconds);
#endif
}

void updateClock() {
  unsigned long now = millis();
  if (now - lastClockTick >= 1000) {
    lastClockTick += 1000;
    currentSeconds++;
    if (currentSeconds >= 60) {
      currentSeconds = 0;
      currentMinutes++;
      if (currentMinutes >= 60) {
        currentMinutes = 0;
        currentHours++;
        if (currentHours >= 24) {
          currentHours = 0;
          currentDay++;
        }
        updateDayPeriod();
      }
    }
  }
}

void updateDayPeriod() {
  if (currentHours >= 6 && currentHours < 12) {
    currentPeriod = PERIOD_MORNING;
  } else if (currentHours >= 12 && currentHours < 18) {
    currentPeriod = PERIOD_AFTERNOON;
  } else if (currentHours >= 18 && currentHours < 22) {
    currentPeriod = PERIOD_EVENING;
  } else {
    currentPeriod = PERIOD_NIGHT;
  }
}
