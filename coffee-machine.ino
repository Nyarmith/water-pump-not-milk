#include <SoftwareSerial.h>
#include "coffee-pins.hh"

class LevelSensor
{
public:
    LevelSensor(int pin) : sensePin_(pin) {}

    bool checkState()
    {
        bool measured = digitalRead(sensePin_);
        measured = failOpen_ ? !measured : measured;
        
        uint64_t curTime = millis();
        
        if (measured == curState_)
        {
            transitionTime = curTime;
        }

        if (curTime - transitionTime_ > debounceTime_)
        {
            transitionTime = curTime; 
            curState_ = measured;
        }
        
        return curState_;
    }

private:
    int sensePin_;
    bool failOpen_;
    uint64_t transitionTime_;
    bool curState_;
    static constexpr debounceTime_ = 250; // millis
};


enum class States { Standby, Pump, Failure };

class StateMachine
{
private:
    State current{Standby};

    static constexpr uint64_t maxFillTime_;
    static constexpr uint64_t minEmptyTime_;
    uint64_t elapsedPump_; 
    uint64_t elapsedLastFill_; 

    LevelSensor lowSensor_{LOW_LEVEL_PIN};
    LevelSensor highSensor_{HIGH_LEVEL_PIN};
    LevelSensor overflowSensor_{FAILSAFE_LEVEL_PIN};


    bool checkFailures()
    {
        // how long we've been pumping water
        if (elampsedPump_ > maxFillTime_)
            return true;

        // checking the sanity of our switch state

        // bad state: any time the sensors above see something the sensors below do
        // or the overflow sensor is on
        if (overflowSensor_.checkState() || (highSensor_.checkState() && !lowSensor_.checkState()))
            return true;

        return false;
    }
public:
    void update(uint64_t dt)
    {
        digitalRead(LOW_LEVEL_PIN);

        if (checkFailures())
            transition(States::Failure);

        stateop(dt);
    };

    void transition(State next)
    {
        // on certain transitions we enable/disable stuff
        auto prev = current;

        // transition functions
        if (prev == States::Failure) //can't escape failure
            return;
        else if (prev == States::Standby && next == States::Pump)
        {
            elapsedPump_ = 0;
            digitalWrite(WATER_PUMP_PIN, FILL);
        }
        else if (prev == States::Pump)
        {
            elapsedLastFill_ = 0;
            digitalWrite(WATER_PUMP_PIN, STOP);
        }
        current = dst;
    };

    void stateop(uint64_t dt)
    {
        switch(current)
        {
            case States::Standby:
                return;
            case States::Pump:
                elapsedPump_ += dt;
                // keep pumping,
                // update dt for failsafes or something
                return;
            case States::Failure:
                // blink error light
                return;
        }
    };
};


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
