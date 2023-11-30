#ifdef BOARD_ATTINY85
#define NANO_PRINT(X) // Serail.print(X)
#define NANO_PRINTLN(X) // Serail.println(X)
#define GREENLED_PIN 4
#define BUTTON_PIN 3
#define STRIP_PIN 0     // пин ленты
#else
#define NANO_PRINT(X) Serial.print(X)
#define NANO_PRINTLN(X) Serial.println(X)
#define GREENLED_PIN 3
#define BUTTON_PIN  2
#define STRIP_PIN 13     // пин ленты
#endif

#define NEZUMIKUN_LED_MANAGER_EFFECTS_NUMBER 8
// ===== ЦВЕТОВАЯ ГЛУБИНА =====
// 1, 2, 3 (байт на цвет)
// на меньшем цветовом разрешении скетч будет занимать в разы меньше места,
// но уменьшится и количество оттенков и уровней яркости!
// дефайн делается ДО ПОДКЛЮЧЕНИЯ БИБЛИОТЕКИ
// без него будет 3 байта по умолчанию
#define COLOR_DEBTH 3
#include <microLED.h>   // подключаем библу
#include <FastLEDsupport.h> // вкл поддержку

#ifdef BOARD_ATTINY85
#define NUMLEDS 100      // кол-во светодиодов
#define BRIGHTNESS 255
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_HIGH> strip;
#else
#define NUMLEDS 30      // кол-во светодиодов на тестовой ленте
#define BRIGHTNESS 128
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER, SAVE_MILLIS> strip;
#endif

inline void _fill(mData color) {
  strip.fill(color);
  strip.show();
}

void testOnStart() {
  NANO_PRINTLN("Start test");
  _fill(mRed);  // заливаем красным
  delay(500);
  _fill(mGreen);  // заливаем зелёным
  delay(500);
  _fill(mBlue);  // заливаем синим
  delay(500);
  _fill(mWhite);  // заливаем белым
  delay(500);
  NANO_PRINTLN("Finish test");
}

void setup() {
#ifdef BOARD_ARDUINO_NANO
  Serial.begin(115200);
#endif
  NANO_PRINTLN("Start!!!");
  pinMode(GREENLED_PIN, OUTPUT);
  digitalWrite(GREENLED_PIN, LOW);
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  testOnStart();
}

uint8_t hue = 0; // rotating "base color" used by many of the patterns
uint8_t currentEffectIndex = 0;
uint8_t subMode = 0;
bool demoMode = true;
bool isShowAllModesInDemo = false;
bool first = true;
uint8_t ledGreenState = LOW;
unsigned long now = 0;

void rainbow() {
  static uint8_t skip = 3;
  if (first) {
    NANO_PRINT("Rainbow");
    if ((subMode & 1) == 0) {
      NANO_PRINT(" without Glitter");
    }
    else {
      NANO_PRINT(" with Glitter");
    }
    NANO_PRINTLN();
    first = false;
  }
  if (--skip == 0) skip = 3; else { hue --; return; }
  int glitter = -1;
  if ((subMode & 1) == 1) {
    glitter = (random8() < 80) ? random8(NUMLEDS) : -1;
  }
  for (int i = 0; i < NUMLEDS; i++) {
    if (i == glitter) {
      strip.set(i, mWhite); // блёстка
    } else {
      strip.set(i, mWheel8((i + hue) * 255 / NUMLEDS)); // выводим радугу
    }
  }
  strip.show();
}

void rainbowNoise() {
  if (first) {
    NANO_PRINTLN("Rainbow with noise");
    first = false;
  }
  static int counter = 0;
  static byte counter2 = 0;
  counter += 10;
  counter2++;
  for (int i = 0; i < NUMLEDS; i++) strip.set(i, mWheel8(counter2 + inoise8(i * 50, counter)));
  strip.show();
}

inline void _fadeStrip(uint8_t fadeValue) {
  for( int i = 0; i < NUMLEDS; i++) { //9948
    strip.fade(i, fadeValue);
  }
}

void sinelon() {
  if (first) {
    NANO_PRINTLN("Sinelon");
    first = false;
    strip.clear();
  }
  _fadeStrip(25 * 30 / NUMLEDS);
  uint8_t x = beatsin8(13, 0, NUMLEDS - 1);
  mData temp = strip.get(x);
  CRGB color = CRGB(temp.r, temp.g, temp.b);
  color += CHSV(hue, 255, 192);
  temp = CRGBtoData(color);
  strip.set(x, temp);
  strip.show();
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  if (first) {
    NANO_PRINTLN("Confetti");
    first = false;
    strip.clear();
  }
  _fadeStrip(25 * 30 / NUMLEDS);
  uint8_t x = random8(NUMLEDS);
  strip.set(x, getBlend(1, 2, strip.get(x), mWheel8(hue + random8(64), 255)));
  strip.show();
}

void juggle() {
  if (first) {
    NANO_PRINTLN("Juggle");
    first = false;
    strip.clear();
  }
  // eight colored dots, weaving in and out of sync with each other
  _fadeStrip(25 * 30 / NUMLEDS);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    uint8_t x = beatsin8(i + 7, 0, NUMLEDS - 1);
    mData temp = strip.get(x);
    CRGB color = CRGB(temp.r, temp.g, temp.b);
    color |= CHSV(dothue, 200, 255);
    temp = CRGBtoData(color);
    strip.set(x, temp);
    dothue += 32;
  }
  strip.show();
}

void bpm()
{
  if (first) {
    NANO_PRINTLN("Beats-Per-Minute (BPM)");
    first = false;
    strip.clear();
  }
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUMLEDS; i++) { //9948
    strip.set(i, CRGBtoData(ColorFromPalette(palette, hue + (i * 2), beat - hue + (i * 10))));
  }
  strip.show();
}

#ifdef BOARD_ATTINY85
#define NEZUMIKUN_LED_MANAGER_PALLETES_NUMBER 8
#else
#define NEZUMIKUN_LED_MANAGER_PALLETES_NUMBER 8
#endif
DEFINE_GRADIENT_PALETTE ( aurora_gp ) {
  0,  17, 177,  13,    //Greenish
  64, 121, 242,   5,    //Greenish
  128,  25, 173, 121,    //Turquoise
  192, 250,  77, 127,    //Pink
  255, 171, 101, 221     //Purple
};
DEFINE_GRADIENT_PALETTE( xmas_gp ) {
  0,   0, 12,  0,
  40,   0, 55,  0,
  66,   1, 117,  2,
  77,   1, 84,  1,
  81,   0, 55,  0,
  119,   0, 12,  0,
  153,  42,  0,  0,
  181, 121,  0,  0,
  204, 255, 12,  8,
  224, 121,  0,  0,
  244,  42,  0,  0,
  255,  42,  0,  0
};

void perlinNoise() {
  static uint16_t x = 0;
  static uint16_t y = 0;
  CRGB color;
  if (first) {
    NANO_PRINT("Perlin noise (Palette ");
    NANO_PRINT(subMode % NEZUMIKUN_LED_MANAGER_PALLETES_NUMBER);
    NANO_PRINT("/");
    NANO_PRINT(NEZUMIKUN_LED_MANAGER_PALLETES_NUMBER);
    NANO_PRINTLN(")");
    first = false;
  }
  for(uint8_t i = 0; i < NUMLEDS; i++) {
    switch(subMode % NEZUMIKUN_LED_MANAGER_PALLETES_NUMBER) {
      case 0:
        color = ColorFromPalette((CRGBPalette16) xmas_gp, inoise8(i * 40, y));
        break;
      case 1:
        color = ColorFromPalette(HeatColors_p, inoise8(i * 40, y));
        break;
      case 2:
        color = ColorFromPalette(PartyColors_p, inoise8(i * 40, y));
        break;
      case 3:
        color = ColorFromPalette(RainbowColors_p, inoise8(i * 40, y));
        break;
      case 4:
        color = ColorFromPalette(LavaColors_p, inoise8(i * 40, y));
        break;
      case 5:
        color = ColorFromPalette(CloudColors_p, inoise8(i * 40, y));
        break;
      case 6:
        color = ColorFromPalette((CRGBPalette16) aurora_gp, inoise8(i * 40, y));
        break;
#ifndef BOARD_ATTINY85
#endif
    }
    strip.set(i, CRGBtoData(color));
  }
  strip.show();
  x++;
  y += 10;
}

inline mData _getRussianFlagGradient(uint8_t x) {
  if (x <= 10 * NUMLEDS / 30) return mRed;
  if (x <= 20 * NUMLEDS / 30) return mBlue;
  return mWhite;
}

inline mData _getXMassGradient(uint8_t x) {
  return (((x / (NUMLEDS / 8)) & 1) == 0) ? mGreen : mRed;
}

void gradient() {
  // указываем цвет каждой точки
  if (first) {
    NANO_PRINT("Gradient ");
    if ((subMode & 1) == 0)
      NANO_PRINT ("Russian flag");
    else 
      NANO_PRINT ("XMass");
    NANO_PRINTLN();
    first = false;
  }
  static uint8_t skip = 10;
  if (--skip == 0) skip = 10; else { hue --; return; }
  for (int i = 0; i < NUMLEDS; i++) {
    if ((subMode & 1) == 0)
      strip.set(i, _getRussianFlagGradient((i + hue) % NUMLEDS));
    else 
      strip.set(i, _getXMassGradient((i + hue) % NUMLEDS));
  }
  strip.show();
}

void nextEffect() {
  uint8_t prevEffectIndex = currentEffectIndex;
  NANO_PRINT("Next Effect: ");
  NANO_PRINT(currentEffectIndex);
  NANO_PRINT(" -> ");
  if (demoMode) {
    NANO_PRINT("[DEMO] ");
    if (!isShowAllModesInDemo) {
      NANO_PRINT("[SHOW ALL] ");
      currentEffectIndex = (currentEffectIndex + 1) % NEZUMIKUN_LED_MANAGER_EFFECTS_NUMBER;
      NANO_PRINT(currentEffectIndex);
      if (currentEffectIndex == 0) {
        isShowAllModesInDemo = true;
        NANO_PRINT(" [SHOW ALL OFF]");
      }
    } else {
      if ((random8() & 1) == 1) {
        subMode = random8();
        currentEffectIndex = random8(NEZUMIKUN_LED_MANAGER_EFFECTS_NUMBER);
      }
      NANO_PRINT(currentEffectIndex);
    }
  } else {
    subMode = random8();
    currentEffectIndex = (currentEffectIndex + 1) % NEZUMIKUN_LED_MANAGER_EFFECTS_NUMBER;
    NANO_PRINT(currentEffectIndex);
  }
  NANO_PRINTLN("");
  first = (prevEffectIndex != currentEffectIndex);
}

void checkButtonPush() {
  static bool hold = false;
  static bool buttonState = false;
  static uint32_t buttonTimer = 0;
  bool temp = digitalRead(BUTTON_PIN);
  uint32_t delta = now - buttonTimer;
  if (temp && !buttonState && delta > 100) {
    buttonState = true;
    buttonTimer = now;
    hold = false;
    NANO_PRINTLN("Button is pressed");
  }
  else if (temp && buttonState && delta > 1000) {
    demoMode = true;
    if (!hold) {
      hold = true;
      NANO_PRINTLN("Button is holded");
    }
  }
  else if (!temp && buttonState && delta > 100) {
    buttonState = false;
    buttonTimer = now;
    if (!hold) {
      if (!demoMode) {
        nextEffect();
      }
      demoMode = false;
      ledGreenState = LOW;
    }
    hold = false;
    NANO_PRINTLN("Button is released");
  }
}

void ledGreenTouch() {
  static unsigned long prevTime = 0;
  if (now - prevTime < 500) return;
  if (demoMode) {
    ledGreenState = (ledGreenState == LOW) ? HIGH : LOW;
  }
  digitalWrite(GREENLED_PIN, ledGreenState);
  prevTime = now;
}

void loop() {
  static unsigned long prevStripTime = 0;
  static unsigned long prevEffectTime = 0;
  now = millis();
  checkButtonPush();
  ledGreenTouch();
  if (now - prevStripTime >= 10) { 
    // currentEffectIndex = 4;
    switch (currentEffectIndex) {
      case 0: rainbow(); break;
      case 1: sinelon(); break;
      case 2: rainbowNoise(); break;
      case 3: confetti(); break;
      case 4: gradient(); break;
      case 5: bpm(); break;
      case 6: perlinNoise(); break;
      case 7: juggle(); break;
     }
    hue++;
    prevStripTime = now;
  }
  if (now - prevEffectTime >= 20000) { 
    if (demoMode) {
      nextEffect();
    } else {
      uint8_t prevSubMode = subMode;
      subMode = random8();
      first = (currentEffectIndex == 3) && (prevSubMode != subMode);
    }
    prevEffectTime = now;
  }
}
