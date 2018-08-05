// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_COUNT 4
#define PIXEL_PIN D0
#define PIXEL_TYPE WS2812B


// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
//               note: if not specified, D2 is selected for you.
// Parameter 3 = pixel type [ WS2812, WS2812B, WS2811, TM1803 ]
//               note: if not specified, WS2812B is selected for you.
//               note: RGB order is automatically applied to WS2811,
//                     WS2812/WS2812B/TM1803 is GRB order.
//
// 800 KHz bitstream 800 KHz bitstream (most NeoPixel products ...
//                         ... WS2812 (6-pin part)/WS2812B (4-pin part) )
//
// 400 KHz bitstream (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//                   (Radio Shack Tri-Color LED Strip - TM1803 driver
//                    NOTE: RS Tri-Color LED's are grouped in sets of 3)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

const int HIGH_STRIKE_LIKELIHOOD = 5;
const int LOW_STRIKE_LIKELIHOOD = 10;
int currentDataPoint = 0;
int chance = LOW_STRIKE_LIKELIHOOD;

// Simple moving average plot
int NUM_Y_VALUES = 17;

float yValues[] = {
  0,
  7,
  10,
  9,
  7.1,
  7.5,
  7.4,
  12,
  15,
  10,
  0,
  3,
  3.5,
  4,
  1,
  7,
  1
};

float simple_moving_average_previous = 0;
float random_moving_average_previous = 0;

void setup() {

    strip.begin(); // Sends the start protocol for the LEDs.
    strip.show(); // Initialize all pixels to 'off'
    
    Particle.function("lightning", triggerWeather);
}

void loop() {

}

int triggerWeather(String command) {
  bool strike = false;
  if (command==String("f")) {
   strike = true;
 } else if (random(100) < chance) {
   strike = true;
 }
    if (strike) {
        int led = random(PIXEL_COUNT);
        for (int i = 0; i < 10; i++) {
        // Use this line to keep the lightning focused in one LED.
        // lightningStrike(led):
        // Use this line if you want the lightning to spread out among multiple LEDs.
        lightningStrike(random(PIXEL_COUNT));
        }
        // Once there's been one strike, I make it more likely that there will be a second.
        chance = HIGH_STRIKE_LIKELIHOOD;
    } else {
        chance = LOW_STRIKE_LIKELIHOOD;
    }
  turnAllPixelsOff();
  delay(1000);
}

void turnAllPixelsOff() {
  for (int i = 0; i < PIXEL_COUNT; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}

void lightningStrike(int pixel) {
  float brightness = simple_moving_average();
  float scaledWhite = abs(brightness*500);
  
  strip.setPixelColor(pixel, strip.Color(scaledWhite, scaledWhite, scaledWhite));
  strip.show();
  delay(random(5, 100));
  currentDataPoint++;
  currentDataPoint = currentDataPoint%NUM_Y_VALUES;
}


// https://en.wikipedia.org/wiki/Moving_average#Simple_moving_average
float simple_moving_average() {
  uint32_t startingValue = currentDataPoint;
  uint32_t endingValue = (currentDataPoint+1)%NUM_Y_VALUES;
  float simple_moving_average_current = simple_moving_average_previous + 
                                  (yValues[startingValue])/NUM_Y_VALUES - 
                                  (yValues[endingValue])/NUM_Y_VALUES;

  simple_moving_average_previous = simple_moving_average_current;
  return simple_moving_average_current;
}
