#include <NewPing.h>
#include <SoftwareSerial.h>


/* failsafes:
 * - how long we've been pumping water
 * - how long it's been since it was last full
 * - checking that our switches are in a sane state
 */

constexpr unsigned ECHO_PIN 2;
constexpr unsigned RELAY_PIN 4;
constexpr unsigned FILL HIGH;
constexpr unsigned STOP LOW;
constexpr unsigned MAX_DISTANCE 200;
constexpr unsigned LOOP_INTERVAL 733;
constexpr unsigned MAX_FLOW_TIME 24000;
constexpr unsigned DEBOUNCE 500;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
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
  unsigned int distance = sonar.ping_cm();
  Serial.print(distance);
  Serial.println("cm");
  if(debounce < LOOP_INTERVAL)
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
//  if(flow_time > MAX_FLOW_TIME)
//    while(1)
//        Serial.println("bad");
}
