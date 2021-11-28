
#include <FastGPIO.h>
#define APA102_USE_FAST_GPIO

#include <APA102.h>
#include <EEPROM.h>

#include <SoftwareSerial.h>
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin

#include "DHT.h"
#define DHTPIN 8
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

#include <arduino-timer.h>
auto timer = timer_create_default(); // create a timer with default settings

const uint8_t dataPin = 2;
const uint8_t clockPin = 3;
const uint8_t sw1Pin = 4;
const uint8_t sw2Pin = 5;

APA102<dataPin, clockPin> ledStrip;

#define LED_COUNT 24
rgb_color colors[LED_COUNT];

// Set the brightness to use (the maximum is 31).
uint8_t globalBrightness = 1;
uint8_t brightnessStep = 5;

String indata;

#define NUM_STATES  7  // number of patterns to cycle through

// system timer, incremented by one every time through the main loop
unsigned int loopCount = 0;

unsigned int seed = 0;  // used to initialize random number generator

// enumerate the possible patterns in the order they will cycle
enum Pattern {
  AllOff = 0,
  //WarmWhiteShimmer = 0,
  //RandomColorWalk = 1, //x
  TraditionalColors = 1,
  //ColorExplosion = 3,//x
  Gradient = 2,
  //BrightTwinkle = 5,//x
  Collision = 3
};
unsigned char pattern = AllOff;
unsigned int maxLoops;  // go to next state when loopCount >= maxLoops

bool send_temp(void *) {
  float t = dht.readTemperature();
  HC12.println(String(t, 2));
  return true; // repeat? true
}

// initialization stuff
void setup()
{
  pinMode(sw1Pin, OUTPUT);
  pinMode(sw2Pin, OUTPUT);
  Serial.begin(9600);
  HC12.begin(9600);
  HC12.setTimeout(10);
  // initialize the random number generator with a seed obtained by
  // summing the voltages on the disconnected analog inputs
  for (int i = 0; i < 8; i++)
  {
    seed += analogRead(i);
  }
  seed += EEPROM.read(0);  // get part of the seed from EEPROM
  randomSeed(seed);
  // save a random number in EEPROM to be used for random seed
  // generation the next time the program runs
  EEPROM.write(0, random(256));
  dht.begin();
  timer.every(1000, send_temp);
}

/********************************************************************/
// main loop
void loop()
{
  timer.tick();
  
  uint8_t startTime = millis();
  
  handleNextPatternButton();

  if (loopCount == 0)
  {
    // whenever timer resets, clear the LED colors array (all off)
    for (int i = 0; i < LED_COUNT; i++)
    {
      colors[i] = rgb_color(0, 0, 0);
    }
  }

  // call the appropriate pattern routine based on state; these
  // routines just set the colors in the colors array
  switch (pattern)
  {

    case TraditionalColors:
      // repeating pattern of red, green, orange, blue, magenta that
      // slowly moves for 400 loopCounts
      maxLoops = 400;
      traditionalColors();
      break;

    case Gradient:
      // red -> white -> green -> white -> red ... gradiant that scrolls
      // across the strips for 250 counts; this pattern is overlaid with
      // waves of dimness that also scroll (at twice the speed)
      maxLoops = 250;
      gradient();
      delay(6);  // add an extra 6ms delay to slow things down
      break;

    case Collision:
      // colors grow towards each other from the two ends of the strips,
      // accelerating until they collide and the whole strip flashes
      // white and fades; this repeats until the function indicates it
      // is done by returning 1, at which point we stop keeping maxLoops
      // just ahead of loopCount
      if (!collision())
      {
        maxLoops = loopCount + 2;
      }
      break;
  }

  // update the LED strips with the colors in the colors array
  ledStrip.write(colors, LED_COUNT, globalBrightness);

  // Make sure that loops are not too fast there are a small number
  // of LEDs and the colors are calculated quickly.
  while ((uint8_t)(millis() - startTime) < 20) { }
  loopCount++;  // increment our loop counter/timer.
}
/********************************************************************/


// This function detects if the optional next pattern button is pressed
// (connecting the pin to ground) and advances to the next pattern
// in the cycle if so.  It also debounces the button.
void handleNextPatternButton()
{
  if  (HC12.available() > 0) {

    indata = HC12.readStringUntil('\n');
    Serial.println(indata);
    //loopCount = 0;  // reset timer
    if (indata == "sw1On") digitalWrite(sw1Pin, HIGH);
    else if (indata == "sw1Off") digitalWrite(sw1Pin, LOW);
    else if (indata == "sw2On") digitalWrite(sw2Pin, HIGH);
    else if (indata == "sw2Off") digitalWrite(sw2Pin, LOW);
    switch (int(indata[0])) {
      case '#':
        if (globalBrightness + brightnessStep <= 31) globalBrightness += brightnessStep;
        break;
      case '*':
        if (globalBrightness - brightnessStep >= 1) globalBrightness -= brightnessStep;
        break;
      case '1': pattern = 1; break;
      case '2': pattern = 2; break;
      case '3': pattern = 3; break;
      case '0': pattern = 0; loopCount = 0; 
      default: break;
    }
  }
}

// This function applies a random walk to val by increasing or
// decreasing it by changeAmount or by leaving it unchanged.
// val is a pointer to the byte to be randomly changed.
// The new value of val will always be within [0, maxVal].
// A walk direction of 0 decreases val and a walk direction of 1
// increases val.  The directions argument specifies the number of
// possible walk directions to choose from, so when directions is 1, val
// will always decrease; when directions is 2, val will have a 50% chance
// of increasing and a 50% chance of decreasing; when directions is 3,
// val has an equal chance of increasing, decreasing, or staying the same.
void randomWalk(unsigned char *val, unsigned char maxVal, unsigned char changeAmount, unsigned char directions)
{
  unsigned char walk = random(directions);  // direction of random walk
  if (walk == 0)
  {
    // decrease val by changeAmount down to a min of 0
    if (*val >= changeAmount)
    {
      *val -= changeAmount;
    }
    else
    {
      *val = 0;
    }
  }
  else if (walk == 1)
  {
    // increase val by changeAmount up to a max of maxVal
    if (*val <= maxVal - changeAmount)
    {
      *val += changeAmount;
    }
    else
    {
      *val = maxVal;
    }
  }
}


// This function fades val by decreasing it by an amount proportional
// to its current value.  The fadeTime argument determines the
// how quickly the value fades.  The new value of val will be:
//   val = val - val*2^(-fadeTime)
// So a smaller fadeTime value leads to a quicker fade.
// If val is greater than zero, val will always be decreased by
// at least 1.
// val is a pointer to the byte to be faded.
void fade(unsigned char *val, unsigned char fadeTime)
{
  if (*val != 0)
  {
    unsigned char subAmt = *val >> fadeTime;  // val * 2^-fadeTime
    if (subAmt < 1)
      subAmt = 1;  // make sure we always decrease by at least 1
    *val -= subAmt;  // decrease value of byte pointed to by val
  }
}

// ***** PATTERN TraditionalColors *****
// This function creates a repeating patern of traditional Christmas
// light colors: red, green, orange, blue, magenta.
// Every fourth LED is colored, and the pattern slowly moves by fading
// out the current set of lit LEDs while gradually brightening a new
// set shifted over one LED.
void traditionalColors()
{
  // loop counts to leave strip initially dark
  const unsigned char initialDarkCycles = 10;
  // loop counts it takes to go from full off to fully bright
  const unsigned char brighteningCycles = 20;

  if (loopCount < initialDarkCycles)  // leave strip fully off for 20 cycles
  {
    return;
  }

  // if LED_COUNT is not an exact multiple of our repeating pattern size,
  // it will not wrap around properly, so we pick the closest LED count
  // that is an exact multiple of the pattern period (20) and is not smaller
  // than the actual LED count.
  unsigned int extendedLEDCount = (((LED_COUNT - 1) / 20) + 1) * 20;

  for (int i = 0; i < extendedLEDCount; i++)
  {
    unsigned char brightness = (loopCount - initialDarkCycles) % brighteningCycles + 1;
    unsigned char cycle = (loopCount - initialDarkCycles) / brighteningCycles;

    // transform i into a moving idx space that translates one step per
    // brightening cycle and wraps around
    unsigned int idx = (i + cycle) % extendedLEDCount;
    if (idx < LED_COUNT)  // if our transformed index exists
    {
      if (i % 4 == 0)
      {
        // if this is an LED that we are coloring, set the color based
        // on the LED and the brightness based on where we are in the
        // brightening cycle
        switch ((i / 4) % 5)
        {
          case 0:  // red
            colors[idx].red = 200 * brightness / brighteningCycles;
            colors[idx].green = 10 * brightness / brighteningCycles;
            colors[idx].blue = 10 * brightness / brighteningCycles;
            break;
          case 1:  // green
            colors[idx].red = 10 * brightness / brighteningCycles;
            colors[idx].green = 200 * brightness / brighteningCycles;
            colors[idx].blue = 10 * brightness / brighteningCycles;
            break;
          case 2:  // orange
            colors[idx].red = 200 * brightness / brighteningCycles;
            colors[idx].green = 120 * brightness / brighteningCycles;
            colors[idx].blue = 0 * brightness / brighteningCycles;
            break;
          case 3:  // blue
            colors[idx].red = 10 * brightness / brighteningCycles;
            colors[idx].green = 10 * brightness / brighteningCycles;
            colors[idx].blue = 200 * brightness / brighteningCycles;
            break;
          case 4:  // magenta
            colors[idx].red = 200 * brightness / brighteningCycles;
            colors[idx].green = 64 * brightness / brighteningCycles;
            colors[idx].blue = 145 * brightness / brighteningCycles;
            break;
        }
      }
      else
      {
        // fade the 3/4 of LEDs that we are not currently brightening
        fade(&colors[idx].red, 3);
        fade(&colors[idx].green, 3);
        fade(&colors[idx].blue, 3);
      }
    }
  }
}

// ***** PATTERN Gradient *****
// This function creates a scrolling color gradient that smoothly
// transforms from red to white to green back to white back to red.
// This pattern is overlaid with waves of brightness and dimness that
// scroll at twice the speed of the color gradient.
void gradient()
{
  unsigned int j = 0;

  // populate colors array with full-brightness gradient colors
  // (since the array indices are a function of loopCount, the gradient
  // colors scroll over time)
  while (j < LED_COUNT)
  {
    // transition from red to green over 8 LEDs
    for (int i = 0; i < 8; i++)
    {
      if (j >= LED_COUNT) {
        break;
      }
      colors[(loopCount / 2 + j + LED_COUNT) % LED_COUNT] = rgb_color(160 - 20 * i, 20 * i, (160 - 20 * i) * 20 * i / 160);
      j++;
    }
    // transition from green to red over 8 LEDs
    for (int i = 0; i < 8; i++)
    {
      if (j >= LED_COUNT) {
        break;
      }
      colors[(loopCount / 2 + j + LED_COUNT) % LED_COUNT] = rgb_color(20 * i, 160 - 20 * i, (160 - 20 * i) * 20 * i / 160);
      j++;
    }
  }

  // modify the colors array to overlay the waves of dimness
  // (since the array indices are a function of loopCount, the waves
  // of dimness scroll over time)
  const unsigned char fullDarkLEDs = 10;  // number of LEDs to leave fully off
  const unsigned char fullBrightLEDs = 5;  // number of LEDs to leave fully bright
  const unsigned char cyclePeriod = 14 + fullDarkLEDs + fullBrightLEDs;

  // if LED_COUNT is not an exact multiple of our repeating pattern size,
  // it will not wrap around properly, so we pick the closest LED count
  // that is an exact multiple of the pattern period (cyclePeriod) and is not
  // smaller than the actual LED count.
  unsigned int extendedLEDCount = (((LED_COUNT - 1) / cyclePeriod) + 1) * cyclePeriod;

  j = 0;
  while (j < extendedLEDCount)
  {
    unsigned int idx;

    // progressively dim the LEDs
    for (int i = 1; i < 8; i++)
    {
      idx = (j + loopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount) {
        return;
      }
      if (idx >= LED_COUNT) {
        continue;
      }

      colors[idx].red >>= i;
      colors[idx].green >>= i;
      colors[idx].blue >>= i;
    }

    // turn off these LEDs
    for (int i = 0; i < fullDarkLEDs; i++)
    {
      idx = (j + loopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount) {
        return;
      }
      if (idx >= LED_COUNT) {
        continue;
      }

      colors[idx].red = 0;
      colors[idx].green = 0;
      colors[idx].blue = 0;
    }

    // progressively bring these LEDs back
    for (int i = 0; i < 7; i++)
    {
      idx = (j + loopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount) {
        return;
      }
      if (idx >= LED_COUNT) {
        continue;
      }

      colors[idx].red >>= (7 - i);
      colors[idx].green >>= (7 - i);
      colors[idx].blue >>= (7 - i);
    }

    // skip over these LEDs to leave them at full brightness
    j += fullBrightLEDs;
  }
}

// ***** PATTERN Collision *****
// This function spawns streams of color from each end of the strip
// that collide, at which point the entire strip flashes bright white
// briefly and then fades.  Unlike the other patterns, this function
// maintains a lot of complicated state data and tells the main loop
// when it is done by returning 1 (a return value of 0 means it is
// still in progress).
unsigned char collision()
{
  const unsigned char maxBrightness = 180;  // max brightness for the colors
  const unsigned char numCollisions = 5;  // # of collisions before pattern ends
  static unsigned char state = 0;  // pattern state
  static unsigned int count = 0;  // counter used by pattern

  if (loopCount == 0)
  {
    state = 0;
  }

  if (state % 3 == 0)
  {
    // initialization state
    switch (state / 3)
    {
      case 0:  // first collision: red streams
        colors[0] = rgb_color(maxBrightness, 0, 0);
        break;
      case 1:  // second collision: green streams
        colors[0] = rgb_color(0, maxBrightness, 0);
        break;
      case 2:  // third collision: blue streams
        colors[0] = rgb_color(0, 0, maxBrightness);
        break;
      case 3:  // fourth collision: warm white streams
        colors[0] = rgb_color(maxBrightness, maxBrightness * 4 / 5, maxBrightness >> 3);
        break;
      default:  // fifth collision and beyond: random-color streams
        colors[0] = rgb_color(random(maxBrightness), random(maxBrightness), random(maxBrightness));
    }

    // stream is led by two full-white LEDs
    colors[1] = colors[2] = rgb_color(255, 255, 255);
    // make other side of the strip a mirror image of this side
    colors[LED_COUNT - 1] = colors[0];
    colors[LED_COUNT - 2] = colors[1];
    colors[LED_COUNT - 3] = colors[2];

    state++;  // advance to next state
    count = 8;  // pick the first value of count that results in a startIdx of 1 (see below)
    return 0;
  }

  if (state % 3 == 1)
  {
    // stream-generation state; streams accelerate towards each other
    unsigned int startIdx = count * (count + 1) >> 6;
    unsigned int stopIdx = startIdx + (count >> 5);
    count++;
    if (startIdx < (LED_COUNT + 1) / 2)
    {
      // if streams have not crossed the half-way point, keep them growing
      for (int i = 0; i < startIdx - 1; i++)
      {
        // start fading previously generated parts of the stream
        fade(&colors[i].red, 5);
        fade(&colors[i].green, 5);
        fade(&colors[i].blue, 5);
        fade(&colors[LED_COUNT - i - 1].red, 5);
        fade(&colors[LED_COUNT - i - 1].green, 5);
        fade(&colors[LED_COUNT - i - 1].blue, 5);
      }
      for (int i = startIdx; i <= stopIdx; i++)
      {
        // generate new parts of the stream
        if (i >= (LED_COUNT + 1) / 2)
        {
          // anything past the halfway point is white
          colors[i] = rgb_color(255, 255, 255);
        }
        else
        {
          colors[i] = colors[i - 1];
        }
        // make other side of the strip a mirror image of this side
        colors[LED_COUNT - i - 1] = colors[i];
      }
      // stream is led by two full-white LEDs
      colors[stopIdx + 1] = colors[stopIdx + 2] = rgb_color(255, 255, 255);
      // make other side of the strip a mirror image of this side
      colors[LED_COUNT - stopIdx - 2] = colors[stopIdx + 1];
      colors[LED_COUNT - stopIdx - 3] = colors[stopIdx + 2];
    }
    else
    {
      // streams have crossed the half-way point of the strip;
      // flash the entire strip full-brightness white (ignores maxBrightness limits)
      for (int i = 0; i < LED_COUNT; i++)
      {
        colors[i] = rgb_color(255, 255, 255);
      }
      state++;  // advance to next state
    }
    return 0;
  }

  if (state % 3 == 2)
  {
    // fade state
    if (colors[0].red == 0 && colors[0].green == 0 && colors[0].blue == 0)
    {
      // if first LED is fully off, advance to next state
      state++;

      // after numCollisions collisions, this pattern is done
      return state >= 3 * numCollisions;
    }

    // fade the LEDs at different rates based on the state
    for (int i = 0; i < LED_COUNT; i++)
    {
      switch (state / 3)
      {
        case 0:  // fade through green
          fade(&colors[i].red, 3);
          fade(&colors[i].green, 4);
          fade(&colors[i].blue, 2);
          break;
        case 1:  // fade through red
          fade(&colors[i].red, 4);
          fade(&colors[i].green, 3);
          fade(&colors[i].blue, 2);
          break;
        case 2:  // fade through yellow
          fade(&colors[i].red, 4);
          fade(&colors[i].green, 4);
          fade(&colors[i].blue, 3);
          break;
        case 3:  // fade through blue
          fade(&colors[i].red, 3);
          fade(&colors[i].green, 2);
          fade(&colors[i].blue, 4);
          break;
        default:  // stay white through entire fade
          fade(&colors[i].red, 4);
          fade(&colors[i].green, 4);
          fade(&colors[i].blue, 4);
      }
    }
  }

  return 0;
}
