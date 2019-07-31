class Segment
{
public:
  int start, end, size;
  CRGB *leds;
  Segment(CRGB *l, int s, int e)
  {
    start = s;
    end = e;
    size = e - s;
    leds = l;
  }
  void setAt(int p, CRGB c)
  {
    leds[start + p] = c;
  }
  void fill(CRGB c)
  {
    for (int i = start; i < end; i++)
    {
      leds[i] = c;
    }
  }
  uint16_t lerp(uint8_t x)
  {
    uint16_t r = lerp16by8(start, end, x);
    // ups
    if (r == end)
      r = end - 1;
    // Serial.print(start);
    // Serial.print("->");
    // Serial.print(end);
    // Serial.print(" by ");
    // Serial.print(x);
    // Serial.print(" = ");
    // Serial.println(r);
    return r;
  }
  void lerpTo(uint8_t x, CRGB c)
  {
    for (int i = start; i < lerp(x); i++)
    {
      leds[i] = c;
    }
  }

  void lerpFromTo(uint8_t x, uint8_t y, CRGB c)
  {
    uint8_t a = min(x, y);
    uint8_t b = max(x, y);
    for (int i = lerp(a); i < lerp(b); i++)
    {
      leds[i] = c;
    }
  }

  void lerpFromToReverse(uint8_t x, uint8_t y, CRGB c)
  {
    for (int i = lerp(255 - x); i > lerp(255 - y); i--)
    {
      leds[i] = c;
    }
  }

  void lerpToReverse(uint8_t x, CRGB c)
  {
    for (int i = end; i > lerp(255 - x); i--)
    {
      leds[i] = c;
    }
  }
  void lerpAt(uint8_t x, CRGB c)
  {
    leds[lerp(x)] = c;
  }
  void lerpAtAdd(uint8_t x, CRGB c)
  {
    leds[lerp(x)] += c;
  }
};

#define LEFT_START 0
#define LEFT_END 23
#define FULL_LEFT_END 21
#define FULL_RIGHT_START 21
#define MIDDLE_START 16
#define MIDDLE_END 27
#define RIGHT_START 23
#define RIGHT_END 50

Segment left = Segment(ledsBottom, LEFT_START, LEFT_END);
Segment right = Segment(ledsBottom, RIGHT_START, RIGHT_END);
Segment middle = Segment(ledsBottom, MIDDLE_START, MIDDLE_END);

Segment leftFull = Segment(ledsBottom, LEFT_START, FULL_LEFT_END);
Segment rightFull = Segment(ledsBottom, FULL_RIGHT_START, RIGHT_END);

#define TOP_LEFT_START 0
#define TOP_LEFT_END 23
#define TOP_RIGHT_START 23
#define TOP_RIGHT_END 54

Segment topLeft = Segment(ledsTop, TOP_LEFT_START, TOP_LEFT_END);
Segment topRight = Segment(ledsTop, TOP_RIGHT_START, TOP_RIGHT_END);

Segment bottom = Segment(ledsBottom, LEFT_START, RIGHT_END);
Segment top = Segment(ledsTop, TOP_LEFT_START, TOP_RIGHT_END);
