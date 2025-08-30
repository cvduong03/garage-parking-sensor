#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


// HC-SR04 Pins
#define TRIG_PIN 27   // sends pulse to start measurement
#define ECHO_PIN 26   // receives reflected pulse and measures time


// WS2812B LED Strips
#define LED_PIN 13    // GPIO 13 for LED Data
#define NUM_LEDS 144  // LEDS per meter of strip
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);


// Tracking last color to prevent random updates
uint8_t lastR = 0, lastG = 0, lastB = 0;
unsigned long lastActiveTime = 0;
const int movementThreshold = 3;  // minimum movement threshold to stay on
const unsigned long idleTimeout = 10000;  // 10000ms = 10s
bool ledON = true;

float lastDistance = 0;

void setup() {
  Serial.begin(115200);         // serial communication at 115200bps
  pinMode(TRIG_PIN, OUTPUT);    // trig as output
  pinMode(ECHO_PIN, INPUT);     // echo as input
  strip.begin();
  strip.setBrightness(100);     // (0-255)
  strip.show();   // LEDs initially off
  lastActiveTime = millis();    // start active time
}


float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);    // start a clear with TRIG pin to low 
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);   // send a high pulse to TRIG pin
  delayMicroseconds(10);          // TRIG must be 10 us to start measurement
  digitalWrite(TRIG_PIN, LOW);    // immediately set TRIG back to low

  // duration is time it takes for trig signal to go out and bounce back to echo
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // measure how long ECHO pin is high
  if (duration == 0) return -1; // timeout
  float distance = duration * 0.0343 / 2 / 2.54;     
  // from spec sheet (distance = duration * speed of sound (343 m/s))
  // we convert 343 m/s to cm/us because we used microsecond signals 
  // divide by 2 for round trip (to object and back) to get one way distance
  // divide by 2.54 to convert from cm to inch

  return distance;
}


void setStripColor(uint8_t r, uint8_t g, uint8_t b) {
  if (r == lastR && g == lastG && b == lastB) return; // only assign new color if the color changes

  for (int i = 0; i < NUM_LEDS; i++) {  // assign new color for all LEDs
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();

  lastR = r;
  lastG = g;
  lastB = b;
  ledON = true;
}


void turnOffStrip() {   // turn off strip 
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
  ledON = false;
  lastR = lastG = lastB = 0;
}


void loop() {
  float newDistance = readDistanceCM();

  // If distance jumps drastically, keep previous reading
  if (abs(newDistance - lastDistance) > 24 && newDistance > 72) { // if the change in distance is greater than 2 feet
    newDistance = lastDistance;
  } else {
    // movement detected over 3cm
    if (abs(newDistance - lastDistance) > movementThreshold) {
      lastActiveTime = millis();
    }
    lastDistance = newDistance;
  }

  Serial.print("Filtered Distance: ");
  Serial.print(newDistance);
  Serial.println(" in");

  // power saving mode: if idle for 10 seconds, turn the LEDs off
  if ((millis() - lastActiveTime) > idleTimeout) {
    if (ledON) {
      Serial.println("Idle timeout reached. Turning off LEDs.");
      turnOffStrip();
    }
    delay(100);
    return;
  }

  // led control based on distance
  if (newDistance < 6) {           // set to flash red (too close/stop)
    setStripColor(255, 0, 0);   // set LED red
    delay(500);                 // hold red
    setStripColor(0, 0, 0);     // set LED off
    delay(500);                 // hold off
  } else if (newDistance < 12) {   // set to solid red (1 feet)
    setStripColor(255, 0, 0); 
  } else if (newDistance < 36) {   // set LED orange (3 feet)
    setStripColor(255, 64, 0); 
  } else if (newDistance < 72) {   // set LED yellow (6 feet)
    setStripColor(255, 200, 0); 
  } else if (newDistance >= 72) {  // set LED green past 6 feet
    setStripColor(0, 255, 0); 
  }

  delay(100); // wait 100ms before updating
}
