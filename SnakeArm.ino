// Snake Arm is built for an Arduino Nano
// If DEBUG is defined, Serial.println() is used to debug the program 
#define DEBUG

#include <Bounce2.h>

// Pin Definitions
const int RETRACT_LIMIT_PIN = 2;     // Limit switch for retracted position
const int EXTEND_LIMIT_PIN = 3;      // Limit switch for extended position
const int BALL_SENSOR_PIN = 4;       // Ball detection sensor
const int MOUTH_SOLENOID_PIN = 5;    // Solenoid controlling mouth
const int ARM_SOLENOID_PIN = 6;      // Solenoid controlling arm movement
const int BALL_FLAP_SOLENOID_PIN = 7;// Solenoid controlling ball flap
const int BALL_LED_PIN = 8;          // LED indicator for ball

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

void extendSequence() {
  log("extendSequence");
  
  // 1. Close mouth
  digitalWrite(MOUTH_SOLENOID_PIN, HIGH);
  
  // 2. Turn on ball LED
  digitalWrite(BALL_LED_PIN, HIGH);
  
  // 3. Extend arm
  digitalWrite(ARM_SOLENOID_PIN, HIGH);
  
  // 4. Wait for extend to complete
  while(!extendLimit.isPressed()) {
    extendLimit.update();
  }
}

void retractSequence() {
  log("retractSequence");
  
  // 5. Turn off ball LED
  digitalWrite(BALL_LED_PIN, LOW);
  
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
  pinMode(BALL_LED_PIN, OUTPUT);

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
    
    // 1. Turn off ball LED
    digitalWrite(BALL_LED_PIN, LOW);
    
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

