#include <SoftwareSerial.h>


/* failsafes:
 * - how long we've been pumping water
 * - how long it's been since it was last full
 * - checking that our switches are in a sane state
 */

constexpr unsigned LOW_LEVEL_PIN 2;
constexpr unsigned HIGH_LEVEL_PIN 3;
constexpr unsigned FAILSAFE_LEVEL_PIN 4;
constexpr unsigned WATER_PUMP_ENABLE_PIN 5;

constexpr unsigned FILL HIGH;
constexpr unsigned STOP LOW;

// TODO: Abstract these guys away
// TODO: Make state machine with debounce time for transitions
// TODO: Make low-pass filter
// TODO: Implement project well (good)
constexpr unsigned DEBOUNCE_INTERVAL_MS 733;
constexpr unsigned MAX_FLOW_TIME 24000;
constexpr unsigned DEBOUNCE 500;

unsigned int prev_time = 0;
unsigned int curr_time = 0;
unsigned int flow_time = 0;

void setup() {
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(RELAY_PIN,OUTPUT);
  Serial.begin(9600);
}
void loop() {
  unsigned int now = millis();
  flow_time = millis() - prev_time;
  Serial.print(distance);
  Serial.println("cm");
  if(debounce < DEBOUNCE_INTERVAL_MS)
    return;
  if(distance <= 10) {
    //digitalWrite(RELAY_PIN,STOP);
    Serial.println("STOP!");
    flow_time = 0;    
  }
  if(distance >= 22 && now-prev_time < DEBOUNCE){ 
    //digitalWrite(RELAY_PIN,FILL);
    Serial.println("FILL!");
    prev_time = millis();  
  }                                                                                                        
}
