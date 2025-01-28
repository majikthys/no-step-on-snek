// Snake Arm is built for an Arduino Nano
// If DEBUG is defined, Serial.println() is used to debug the program 
// 
// LED is an array of 61 WS2812B LEDs
// And effect is based on PRIDE2015 by Mark Kriegsman
// https://github.com/FastLED/FastLED
#define DEBUG

#include <Bounce2.h>
#include <FastLED.h>

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// Pin Definitions
const int RETRACT_LIMIT_PIN = 12;     // Limit switch for retracted position
const int EXTEND_LIMIT_PIN = 10;      // Limit switch for extended position
const int BALL_SENSOR_PIN = 8;       // Ball detection sensor

const int MOUTH_SOLENOID_PIN = 19;     // Solenoid controlling mouth
const int ARM_SOLENOID_PIN = 17;       // Solenoid controlling arm movement
const int BALL_FLAP_SOLENOID_PIN = 15; // Solenoid controlling ball flap
#define DATA_PIN 3                     // LED data pin

// FastLED definitions
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    61
#define BRIGHTNESS  255

CRGB leds[NUM_LEDS];

// Create button instances for each input
Bounce2::Button retractLimit = Bounce2::Button();
Bounce2::Button extendLimit = Bounce2::Button();
Bounce2::Button ballSensor = Bounce2::Button();

// Timing constants (in milliseconds)
const unsigned long BALL_FLAP_DELAY = 500;  // Time for ball flap movement
const unsigned long MOUTH_DELAY = 500;      // Time for mouth movement
const unsigned long DEBOUNCE_INTERVAL = 5;  // Debounce interval in ms

void log(const char* message) {
  #ifdef DEBUG
    Serial.println(message);
  #endif
}

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}

void turnOnLEDs() {
  FastLED.setBrightness(BRIGHTNESS);
}

void turnOffLEDs() {
  FastLED.setBrightness(0);
  FastLED.show();
}

void extendSequence() {
  log("extendSequence");
  
  // 1. Close mouth
  digitalWrite(MOUTH_SOLENOID_PIN, HIGH);
  
  // 2. Turn on LEDs
  turnOnLEDs();
  
  // 3. Extend arm
  digitalWrite(ARM_SOLENOID_PIN, HIGH);
  
  // 4. Wait for extend to complete
  while(!extendLimit.isPressed()) {
    extendLimit.update();
    // Always run pride() when extending - no need for ledsOn check
    pride();
    FastLED.show();
  }
}

void retractSequence() {
  log("retractSequence");
  
  // 5. Turn off LEDs
  turnOffLEDs();
  
  // 6. Retract ball flap
  digitalWrite(BALL_FLAP_SOLENOID_PIN, HIGH);
  delay(BALL_FLAP_DELAY);
  
  // 7. Retract arm
  digitalWrite(ARM_SOLENOID_PIN, LOW);
  
  // 8. Wait for retract to complete
  while(!retractLimit.isPressed()) {
    retractLimit.update();
  }
  
  // 9. Extend ball flap
  digitalWrite(BALL_FLAP_SOLENOID_PIN, LOW);
  delay(BALL_FLAP_DELAY);
  
  // 10. Open mouth
  digitalWrite(MOUTH_SOLENOID_PIN, LOW);
  delay(MOUTH_DELAY);
}

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  log("Snake Arm Initializing...");

  // Configure input pins with debouncing
  retractLimit.attach(RETRACT_LIMIT_PIN, INPUT_PULLUP);
  retractLimit.interval(DEBOUNCE_INTERVAL);
  retractLimit.setPressedState(LOW);  // Switch closes to ground
  
  extendLimit.attach(EXTEND_LIMIT_PIN, INPUT_PULLUP);
  extendLimit.interval(DEBOUNCE_INTERVAL);
  extendLimit.setPressedState(LOW);  // Switch closes to ground
  
  ballSensor.attach(BALL_SENSOR_PIN, INPUT_PULLUP);
  ballSensor.interval(DEBOUNCE_INTERVAL);
  ballSensor.setPressedState(LOW);  // Sensor triggers LOW
  
  // Configure output pins
  pinMode(MOUTH_SOLENOID_PIN, OUTPUT);
  pinMode(ARM_SOLENOID_PIN, OUTPUT);
  pinMode(BALL_FLAP_SOLENOID_PIN, OUTPUT);
  
  // Configure FastLED
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);
  turnOffLEDs();

  // Update button states
  retractLimit.update();
  extendLimit.update();
  ballSensor.update();

  // Initial startup sequence
  if (!retractLimit.isPressed()) {
    log("Starting initial sequence...");
    
    // 0. Extend ball flap
    digitalWrite(BALL_FLAP_SOLENOID_PIN, HIGH);
    delay(BALL_FLAP_DELAY);
    
    extendSequence();
    retractSequence();
  } 
  else {
    log("Arm already retracted, resetting state...");
    
    // 1. Turn off LEDs
    turnOffLEDs();
    
    // 2. Retract arm
    digitalWrite(ARM_SOLENOID_PIN, LOW);
    
    // 3. Extend ball flap
    digitalWrite(BALL_FLAP_SOLENOID_PIN, LOW);
    
    // 4. Open mouth
    digitalWrite(MOUTH_SOLENOID_PIN, LOW);
    delay(MOUTH_DELAY);
  }
}

void loop() {
  // Update all button states
  retractLimit.update();
  extendLimit.update();
  ballSensor.update();

  // Wait for ball detection
  if (ballSensor.pressed()) {
    log("Ball detected, starting sequence...");
    
    extendSequence();
    retractSequence();
    
    log("Sequence complete");
  }
}

