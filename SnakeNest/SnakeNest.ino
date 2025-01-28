// Snake Nest is built for an Arduino Nano
// Monitor hand detector pin and turn on solenoid-relay, light-relay, and sound-relay for a duration
// Then turn off solenoid, lights, and sound
#define DEBUG

#include <Bounce2.h>

// Pin Definitions
const int HAND_DETECTOR_PIN = 2;     // Hand detection sensor
const int SOLENOID_PIN = 3;    // Relay controlling solenoid
const int LIGHT_PIN = 4;       // Relay controlling lights
const int SOUND_PIN = 5;       // Relay controlling sound

// Create button instance for hand detector
Bounce2::Button handDetector = Bounce2::Button();

// Timing constants (in milliseconds)
const unsigned long ACTIVATION_DURATION = 2200;  // How long to keep everything activated in ms
const unsigned long DEBOUNCE_INTERVAL = 5;       // Debounce interval in ms

void log(const char* message) {
  #ifdef DEBUG
    Serial.println(message);
  #endif
}

void activateSequence() {
  log("Activating nest");
  
  // Turn on all relays
  digitalWrite(SOLENOID_PIN, HIGH);
  digitalWrite(LIGHT_PIN, HIGH);
  digitalWrite(SOUND_PIN, HIGH);
  
  // Wait for duration
  delay(ACTIVATION_DURATION);
  
  // Turn off all relays
  digitalWrite(SOLENOID_PIN, LOW);
  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(SOUND_PIN, LOW);
  
  log("Nest deactivated");
}

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  log("Snake Nest Initializing...");

  // Configure input pin with debouncing
  handDetector.attach(HAND_DETECTOR_PIN, INPUT_PULLUP);
  handDetector.interval(DEBOUNCE_INTERVAL);
  handDetector.setPressedState(LOW);  // Sensor triggers LOW
  
  // Configure output pins
  pinMode(SOLENOID_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(SOUND_PIN, OUTPUT);
  
  // Ensure everything starts off
  digitalWrite(SOLENOID_PIN, LOW);
  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(SOUND_PIN, LOW);
  
  // Initial button state
  handDetector.update();
}

void loop() {
  // Update hand detector state
  handDetector.update();

  // Check for hand detection
  if (handDetector.pressed()) {
    log("Hand detected, starting sequence...");
    activateSequence();
    log("Sequence complete");
  }
}



