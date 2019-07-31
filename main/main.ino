#include <FastLED.h>

#define BUTTON_PIN 6

#define LED_TYPE WS2811
#define COLOR_ORDER GRB

#define MAX_BRIGHTNESS 120
// #define MAX_BRIGHTNESS 220
#define MIN_BRIGHTNESS 8

#define NUM_LEDS_TOP 54
#define NUM_LEDS_BOTTOM 50

#define NUM_LEDS NUM_LEDS_TOP + NUM_LEDS_BOTTOM

CRGB ledsTop[NUM_LEDS_TOP];
CRGB ledsBottom[NUM_LEDS_BOTTOM];

uint16_t currentDelay = 0;
uint8_t button = 0;
uint8_t shouldClear = 1;

uint8_t autoMode = 1;

int globalP = 23;

#include "segment.h"

// 23  to the midlle

typedef void (*Mode[2])(void);
void none() {}

Mode modes[] = {
    // {colorTestMode, none},
    // {fire, fireSetup},
    // {innerColor, innerColorSetup},
    // {vmoveupOut, vmoveupOutSetup},
    {angle, angleSetup},
    {colorWheel, colorWheelSetup},
    {randomSparks, randomSparksSetup},
    {sinelon, noclearSetup},
    // {topJugle, topJugleSetup},
    {stars, starsSetup},
    // {randomBlue, none},

    {juggle, flashySetup},
    {colorWheelWithSparks, colorWheelWithSparksSetup},
    {bpm, noclearSetup},
    {rainbowSparks, rainbowSparksSetup},

    // {dots, noclearSetup},
    // {colorTestMode, none},
    // {flashy, dotsSetup},
    // {fadeDown, fadeDownSetup},
    // {colorWheelPulsing, none},
};

void clear()
{
  memset(ledsTop, 0, sizeof(ledsTop));
  memset(ledsBottom, 0, sizeof(ledsBottom));
}

// the current active main mode
int8_t currentMode = 0;
int8_t previousMode = 0;
uint8_t currentBrightness = MAX_BRIGHTNESS;

void checkButton()
{
  int bs = 0;
  static uint8_t pressedFor = 0;
  bs = digitalRead(BUTTON_PIN);
  Serial.println(bs);
  if (bs == LOW)
  {
    Serial.println("pressed");
    pressedFor++;
    // nextMode(1);
    autoMode = 0;
  }
  else
  {
    pressedFor = 0;
  }
  if (pressedFor == 2)
  {
    currentMode = -1;
    if (autoMode == 0)
      autoMode = 1;
    else
      autoMode = 0;
    // nextMode(1);
  }
}

uint16_t fps = 0;
uint32_t readBytes = 0;

void showFps()
{
  Serial.println(fps);
  // Serial.println(readBytes);
  fps = 0;
  readBytes = 0;
}

void setup()
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  FastLED.addLeds<NEOPIXEL, 9>(ledsTop, NUM_LEDS_TOP);
  FastLED.addLeds<NEOPIXEL, 10>(ledsBottom, NUM_LEDS_BOTTOM);

  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
  delay(3000); // if we fucked it up - great idea by fastled :D

  //FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
  modes[0][1]();
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextMode(uint8_t dir)
{
  uint8_t newMode;
  newMode = (currentMode + dir) % ARRAY_SIZE(modes);
  FastLED.clear();
  FastLED.show();
  Serial.print("NEW MODE => ");
  Serial.println(newMode);
  setMode(newMode);
}

void setMode(uint8_t mode)
{
  previousMode = currentMode;
  currentMode = mode;
  if (currentMode < 0)
    currentMode = ARRAY_SIZE(modes) - 1;
  setBrightness(MAX_BRIGHTNESS);
  currentDelay = 0;
  shouldClear = 1;
  modes[currentMode][1]();
}

void setBrightness(uint8_t brightness)
{
  currentBrightness = brightness;
  if (currentBrightness > MAX_BRIGHTNESS)
  {
    currentBrightness = MAX_BRIGHTNESS;
  }
  if (currentBrightness < MIN_BRIGHTNESS)
  {
    currentBrightness = MIN_BRIGHTNESS;
  }
  FastLED.setBrightness(currentBrightness);
}

class Spark
{
public:
  int idx;
  int8_t dir;
  float speed;
  float v;
  uint32_t last;
  Spark()
  {
    randomize();
  }
  void randomize()
  {
    dir = 1;
    speed = map8(random8(), 3, 10) / 10.0;
    v = 0;
    idx = 27;
    idx = random8(NUM_LEDS_TOP);
    last = millis();
  }
  void advance(uint32_t t)
  {
    if (last - t == 0)
      return;
    float n = dir * (speed * ((t - last) / 10.0));
    v = v + n;
    last = t;
    if (v > 190 && dir == 1)
    {
      dir *= -1;
    }
    if (v < -5.0)
    {
      randomize();
    }
  }
  void draw(uint8_t base)
  {
    ledsTop[idx] = CHSV(33, 188, base + v);
    // ledsTop[idx] = CHSV(33, 188 + (v / 4), base + v);
  }
};

#define NUM_SPARKS 15

// color wheel
void angle()
{
  static uint8_t hue = 0;
  static Spark sparks[NUM_SPARKS];
  uint32_t t = millis();
  hue++;
  // uint8_t base = map8(beatsin8(5), 100, 190);
  uint8_t base = 60;
  top.fill(CHSV(33, 188, base));
  for (int i = 0; i < NUM_SPARKS; i++)
  {
    sparks[i].advance(t);
    sparks[i].draw(base);
  }
  // top.fill(CHSV(33, 188, map( beat8(10),211,240));
  // top.fill(CHSV(64, 240, 250));
  // for (int j = 0; j < NUM_LEDS_TOP; j++)
  // {
  //   ledsTop[j] = CHSV(8 * j + hue, 192, 255);
  // }
  // for (int j = 0; j < NUM_LEDS_BOTTOM; j++)
  // {
  //   ledsBottom[j] = CHSV(16 * j + hue, 192, 255);
  // }
}

void angleSetup()
{
  currentDelay = 0;
  // shouldClear = false;
  FastLED.setBrightness(250);
}

void colorWheelSetup()
{
  currentDelay = 30;
  FastLED.setBrightness(128);
}

// color wheel
void colorWheel()
{
  static uint8_t hue = 0;
  hue++;
  top.fill(CHSV(64, 240, 80));
  // for (int j = 0; j < NUM_LEDS_TOP; j++)
  // {
  //   ledsTop[j] = CHSV(8 * j + hue, 192, 255);
  // }
  for (int j = 0; j < NUM_LEDS_BOTTOM; j++)
  {
    ledsBottom[j] = CHSV(16 * j + hue, 192, 255);
  }
}

void colorWheelWithSparksSetup()
{
  currentDelay = 15;
  // FastLED.setBrightness(255);
}

void colorWheelWithSparks()
{
  static uint8_t hue = 0;
  hue++;
  for (int j = 0; j < NUM_LEDS_TOP; j++)
  {
    ledsTop[j] = CHSV(16 * j + hue, 192, 64);
  }
  for (int j = 0; j < NUM_LEDS_BOTTOM; j++)
  {
    ledsBottom[j] = CHSV(16 * j + hue, 192, 64);
  }
  int r = random(0, NUM_LEDS_TOP + NUM_LEDS_BOTTOM);
  if (r >= NUM_LEDS_TOP)
    ledsBottom[r - NUM_LEDS_TOP] = CRGB::White;
  else
    ledsTop[r] = CRGB::White;
}

void colorWheelPulsing()
{
  static uint8_t hue = 0;
  static uint8_t pulse = 255;
  static int8_t dir = -1;

  pulse += dir;

  if (pulse < 100)
  {
    dir = 1;
  }
  if (pulse > 253)
  {
    dir = -1;
  }

  hue++;
  for (int j = 0; j < NUM_LEDS_TOP; j++)
  {
    ledsTop[j] = CHSV(16 * j + hue, 192, pulse);
  }
  for (int j = 0; j < NUM_LEDS_BOTTOM; j++)
  {
    ledsBottom[j] = CHSV(16 * j + hue, 192, pulse);
  }
}

void colorWheelUpDownSetup()
{
  currentDelay = 15;
}

void colorWheelUpDown()
{
  // static uint8_t hue = 0;
  // static uint8_t aaa = 0;
  // hue++;
  // uint8_t jj = 0;
  // for(int i = 0; i < 12; i++) {
  //     for(int j = 0; j < 30; j++) {
  //           if( i % 2 == 0) {
  //             jj = 29 - j;
  //         } else {
  //             jj = j;
  //         }
  //         leds[xy60x6(i,jj)] = CHSV((32*(j/3)) + hue,200,255);
  //     }
  // }
}

void fastColorWheel()
{
  // static uint8_t hue = 0;
  // hue++;
  // hue++;
  // hue++;
  // for(int i = 0; i < NUM_STRIPS; i++) {
  //     for(int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
  //         leds[(i*NUM_LEDS_PER_STRIP) + j] = CHSV((32*i) + hue+j,192,255);
  //     }
  // }
}

void randomBlue()
{
  for (int i = 0; i < 2; i++)
  {
    int x = random(0, NUM_LEDS_BOTTOM);
    ledsBottom[x] = CRGB::Blue;
  }
  for (int i = 0; i < 1; i++)
  {
    int x = random(0, NUM_LEDS_TOP);
    ledsTop[x] = CRGB::Blue;
  }
}

byte fadeout[NUM_LEDS_TOP + NUM_LEDS_BOTTOM];
unsigned long fadeStart;

void fadeAllOne()
{
  static uint8_t hue = 0;
  hue += 20;
  fadeStart = micros() + 1000000;
  for (int i = 0; i < NUM_LEDS_BOTTOM; i++)
  {
    ledsBottom[i] = CHSV(hue + i * 5, 240, 255);
    fadeout[i] = random(5, 50);
  }
  for (int i = 0; i < NUM_LEDS_TOP; i++)
  {
    ledsTop[i] = CHSV(hue + i * 5, 240, 255);
    fadeout[i + NUM_LEDS_BOTTOM] = random(5, 50);
  }
}

void fadeDown()
{
  EVERY_N_SECONDS(7) { fadeAllOne(); }

  if (micros() > fadeStart)
  {
    for (int i = 0; i < NUM_LEDS_BOTTOM + NUM_LEDS_TOP; i++)
    {
      if (i >= NUM_LEDS_TOP)
        ledsBottom[i - NUM_LEDS_TOP].fadeToBlackBy(fadeout[i]);
      else
        ledsTop[i].fadeToBlackBy(fadeout[i]);
    }
  }

  // now fade random pixels down
}

void fadeDownSetup()
{
  fadeAllOne();
  shouldClear = false;
  currentDelay = 50;
}

#define NUM_STARS 20

struct star
{
  float speed;
  byte dir;
  float current;
  byte pos;
  byte hue;
  byte sat;
};

typedef struct star Star;
void createStar(Star *s);
byte hueShift;

Star myStars[NUM_STARS];

void createStar(Star *s)
{
  s->speed = float(random(10, 200)) / 80;
  s->current = 0;
  s->dir = 1;
  s->hue = random(128 + hueShift, 224 + hueShift);
  s->sat = random(120, 255);
  if (random(0, 15) == 0)
    s->pos = random(0, NUM_LEDS_TOP - 1);
  else
    s->pos = random(NUM_LEDS_TOP, NUM_LEDS_TOP + NUM_LEDS_BOTTOM - 1);
}

void stars()
{
  for (int i = 0; i < NUM_STARS; i++)
  {
    if (myStars[i].current >= 255)
      myStars[i].dir = 0;

    if (myStars[i].dir)
      myStars[i].current += myStars[i].speed;
    else
      myStars[i].current -= myStars[i].speed;

    if (myStars[i].current < 0)
      createStar(&myStars[i]);

    // render
    if (myStars[i].pos < NUM_LEDS_TOP)
      ledsTop[myStars[i].pos] = CHSV(myStars[i].hue, myStars[i].sat, byte(myStars[i].current));
    else
      ledsBottom[myStars[i].pos - NUM_LEDS_TOP] = CHSV(myStars[i].hue, myStars[i].sat, byte(myStars[i].current));
  }
  EVERY_N_MILLISECONDS(100) { hueShift++; }
}

void starsSetup()
{
  for (int i = 0; i < NUM_STARS; i++)
  {
    createStar(&myStars[i]);
  }
  // FastLED.setBrightness(255);
};

void sparks()
{
  // for(int j = 0; j < 5; j ++) {
  //     leds[random( 0, 360 )] = CRGB::White;
  // }
}

void sparksSetup()
{
  // FastLED.setBrightness(255);
  // currentDelay = 10;
}

void randomSparks()
{
  ledsTop[random(0, NUM_LEDS_TOP)] = CHSV(random(0, 255), 190, 255);
  ledsBottom[random(0, NUM_LEDS_BOTTOM)] = CHSV(random(0, 255), 190, 255);
}

void randomSparksSetup()
{
  // FastLED.setBrightness(255);
  currentDelay = 40;
}

void rainbowSparks()
{
  static uint8_t hue = 0;
  hue++;
  ledsTop[random(0, NUM_LEDS_TOP)] = CHSV(hue, 210, 255);

  for (int j = 0; j < 5; j++)
  {
    ledsBottom[random(0, NUM_LEDS_BOTTOM)] = CHSV(hue, 210, 255);
  }
}

void rainbowSparksSetup()
{
  // FastLED.setBrightness(255);
  currentDelay = 10;
}

void sparksAndRainbow()
{
  // static int8_t dir = -1;
  // static int8_t pos = 15;
  // // for(int j = 0; j < 2; j ++) {
  // EVERY_N_MILLISECONDS(20) { leds[random( 0, 360 )] = CRGB::White; }

  // EVERY_N_MILLISECONDS(50) { leds[random( 360+7, 360+7+54 )] = CRGB::Red; }
  // // }

  // static uint8_t hue = 0;
  // hue++;

  // uint8_t jj = 0;

  // if(pos <= 1) {
  //     dir = 1;
  // }
  // if(pos >= 29) {
  //     dir = -1;
  // }
  // pos += dir;

  // for(int i = 0; i < 12; i ++) {
  //     if( i % 2 == 0) {
  //         jj = 30 - pos;
  //     } else {
  //         jj = pos;
  //     }
  //    uint16_t p = i*30+jj;

  //    leds[p-1] = CHSV((32*i) + hue,192,64);
  //    leds[p] = CHSV((32*i) + hue,192+10,64);
  //    leds[p+1] = CHSV((32*i) + hue,192+20,64);
  // }
}

void sparksAndRainbowSetup()
{
  // FastLED.setBrightness(255);
  // currentDelay = 10;
}

#define SNAKE_LENGTH 5

uint16_t xy60x6(uint8_t x, uint8_t y)
{
  if (y > 29)
  {
    x = x + 6;
    y = 59 - y;
  }
  return (x * 30) + y;
}

uint16_t xy(uint8_t x, uint8_t y)
{
  if (x % 2 != 0)
    return (x % 12) * 30 + (y % 30);
  else
    return (x % 12) * 30 + (29 - (y % 30));
}

void topJugleSetup()
{
  currentDelay = 15;
  shouldClear = false;
  // FastLED.setBrightness(225);
}

void topJugle()
{
  fadeToBlackBy(ledsBottom, NUM_LEDS_BOTTOM, 40);
  fadeToBlackBy(ledsTop, NUM_LEDS_TOP, 40);

  CRGB c = CHSV(beatsin8(50, 0, 255), 240, 255);
  int p = beatsin8(50, 0, 255);
  leftFull.lerpAt(p, c);
  rightFull.lerpAt(255 - p, c);
}

void threeSnakesSetup()
{
  currentDelay = 100;
}

CRGBPalette16 firePal;

void fireSetup()
{
  currentDelay = 60;
  firePal = CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::Grey);
  // FastLED.setBrightness(255);
}

#define COOLING 95
#define SPARKING 80
#define Y 6
#define X 2

#define FIRE_LEDS 12

void fire()
{
  static byte heat[NUM_LEDS_TOP * 2];
  random16_add_entropy(random());

  // cool down
  for (int i = 0; i < FIRE_LEDS; i++)
  {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / FIRE_LEDS) + 2));
  }

  // drift up and difuse
  for (int k = Y - 1; k >= 2; k--)
  {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
  heat[1] = heat[0];

  for (int k = Y; k < (Y * 2) - 2; k++)
  {
    heat[k] = (heat[k + 1] + heat[k + 2] + heat[k + 3]) / 3;
  }

  // heat[15] = heat[0];

  // ignite
  if (random8() < SPARKING)
  {
    int y = random8(3);
    heat[y] = qadd8(heat[y], random8(100, 180));
  }

  if (random8() < SPARKING)
  {
    int y = random8(2);
    heat[(Y * 2) - (y + 1)] = qadd8(heat[(Y * 2) - y], random8(100, 180));
  }

  // map to pixels
  for (int j = 0; j < FIRE_LEDS; j++)
  {
    byte colorindex = scale8(heat[j], 240);
    CRGB color = ColorFromPalette(firePal, colorindex);
    if (j < 6)
    {
      ledsTop[j] = color;
      ledsTop[12 - j] = color;
      ledsTop[12 + j] = color;
      ledsTop[24 - j] = color;
    }
    else
    {
      ledsBottom[LEFT_END - (j - 5)] = color;
      ledsBottom[RIGHT_START + (j - 5)] = color;
    }
  }
}

void colorTestMode()
{
  // for (int i = 0; i < globalP; i++)
  // {
  //   ledsTop[i] = CRGB::Red;
  //   ledsBottom[i] = CRGB::Red;
  // }
  // delay(10);
  ledsBottom[50] = CRGB::Blue;

  left.fill(CRGB::Green);
  right.fill(CRGB::Red);

  topLeft.fill(CRGB::Green);
  topRight.fill(CRGB::Red);

  // middle.fill(CRGB::Red);
  // frontRightInner.fill(CRGB::Yellow);

  // frontLeftOutter.fill(CRGB::Green);
  // frontLeftInner.fill(CRGB::Blue);
  // frontRightOutter.fill(CRGB::Yellow);
  // frontRightInner.fill(CRGB::Red);
  // fill_solid(ledsTop, NUM_LEDS_TOP, CHSV(255, 10, 100));
}

void sinelon()
{
  static uint8_t gHue = 0;
  fadeToBlackBy(ledsBottom, NUM_LEDS_BOTTOM, 20);
  fadeToBlackBy(ledsTop, NUM_LEDS_TOP, 30);
  int pos = beatsin8(13, 0, NUM_LEDS_BOTTOM - 1);
  ledsBottom[pos] += CHSV(gHue, 255, 192);
  int pos2 = beatsin8(30, 0, NUM_LEDS_BOTTOM - 1);
  ledsBottom[pos2] += CHSV(gHue * 2, 255, 192);
  int pos3 = beatsin8(45, 0, NUM_LEDS_TOP - 1);
  ledsTop[pos3] += CHSV(gHue, 255, 192);
  EVERY_N_MILLISECONDS(20) { gHue++; }
}

void sinelonSetup()
{
  shouldClear = false;
  // FastLED.setBrightness(200);
}

void noclearSetup()
{
  shouldClear = false;
  // FastLED.setBrightness(128);
}

void bpm()
{
  static uint8_t gHue = 0;
  uint8_t BeatsPerMinute = 100;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS_TOP; i++)
  {
    ledsTop[i] = ColorFromPalette(palette, gHue + (i * 4), beat - gHue + (i * 10));
  }
  for (int i = 0; i < NUM_LEDS_BOTTOM; i++)
  {
    ledsBottom[i] = ColorFromPalette(palette, gHue + (i * 4), beat - gHue + (i * 10));
  }
  EVERY_N_MILLISECONDS(20) { gHue++; }
}

void juggle()
{
  static uint8_t gHue = 0;
  fadeToBlackBy(ledsBottom, NUM_LEDS_BOTTOM, 20);
  fadeToBlackBy(ledsTop, NUM_LEDS_TOP, 10);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    ledsBottom[beatsin8(i + 7, 0, NUM_LEDS_BOTTOM - 1)] |= CHSV(dothue, 200, 255);
    //    ledsTop[beatsin16(i+7,0,NUM_LEDS_TOP)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  for (int i = 0; i < 4; i++)
  {
    ledsTop[beatsin8(i + 3, 0, NUM_LEDS_TOP - 1)] |= CHSV(dothue, 200, 255);
    dothue += 16;
  }
}

void flashySetup()
{
  shouldClear = false;
  // FastLED.setBrightness(200);
}
void flashy()
{
  static uint8_t gHue = 0;
  fadeToBlackBy(ledsBottom, NUM_LEDS_BOTTOM, 2);
  fadeToBlackBy(ledsTop, NUM_LEDS_TOP, 2);

  EVERY_N_MILLISECONDS(200) { ledsTop[random(0, NUM_LEDS_TOP)] = CHSV(random(0, 255), 240, 128); }
  EVERY_N_MILLISECONDS(100) { ledsBottom[random(0, NUM_LEDS_BOTTOM)] = CHSV(random(0, 255), 240, 128); }

  EVERY_N_SECONDS(60)
  {
    fill_solid(ledsBottom, NUM_LEDS_BOTTOM, CRGB::White);
    fill_solid(ledsTop, NUM_LEDS_TOP, CRGB::White);
  }
}

void dotsSetup()
{
  shouldClear = false;
  currentDelay = 30;
}

void dots()
{
  static uint8_t gHue = 0;
  static uint8_t offset = 0;
  static uint8_t p1 = 0;
  static uint8_t p2 = 5;
  static uint8_t p3 = 10;
  fadeToBlackBy(ledsTop, NUM_LEDS_TOP, 128 / 2);

  int b = beatsin16(30, 0, 100);

  p1 += uint8_t(float(b - 50) / 30.0);
  p2 += uint8_t(float(b - 50) / 30.0);
  p3 += uint8_t(float(b - 50) / 30.0);

  ledsTop[(p1 + offset) % 16] = CHSV(190 + gHue, 240, 128);
  ledsTop[(p2 + offset) % 16] = CHSV(90 + gHue * 2, 240, 128);
  ledsTop[(p3 + offset) % 16] = CHSV(10 + gHue * 3, 240, 128);

  EVERY_N_MILLISECONDS(30) { gHue++; }

  EVERY_N_MILLISECONDS(500)
  {
    currentDelay++;
    if (currentDelay > 200)
      currentDelay = 10;
  }
  EVERY_N_MILLISECONDS(2000) { offset++; }
}

void checkSerial()
{
  if (Serial.available() > 0)
  {
    int c = Serial.read();
    if (c == 'n')
    {
      nextMode(1);
    }
    if (c == 'p')
    {
      globalP++;
      Serial.println(globalP);
    }
  }
}

void loop()
{
  if (shouldClear)
    clear();

  if (autoMode == 1)
  {
    EVERY_N_SECONDS(60) { nextMode(1); }
  }

  modes[currentMode][0]();

  FastLED.show();

  EVERY_N_SECONDS(1) { checkSerial(); }
  // EVERY_N_MILLISECONDS(500) { checkButton(); }
  EVERY_N_SECONDS(90)
  {
    if (autoMode)
      nextMode(1);
  }
  delay(currentDelay);
}
