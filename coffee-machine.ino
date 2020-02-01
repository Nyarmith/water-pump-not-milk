#include <SoftwareSerial.h>
#include "coffee-pins.hh"

enum class States { Standby, Pump, Failure };

class StateMachine
{
private:
    State state_{Standby};

    static constexpr uint64_t maxFillTime_{24000};
    uint64_t elapsedPump_; 
    uint64_t startPump_; 

    LevelSensor lowSensor_{LOW_LEVEL_PIN};
    LevelSensor highSensor_{HIGH_LEVEL_PIN};
    LevelSensor overflowSensor_{FAILSAFE_LEVEL_PIN};


    bool checkFailures()
    {
        if (elampsedPump_ - startPump_ > maxFillTime_)
            return true;

        // Sensor Sanity-Check
        // **** bad state: any time the sensor above sees water while the lower
        // **** sensor doesn't OR any time the overflow sensor detects water
        if (overflowSensor_.checkState() || (highSensor_.checkState() && !lowSensor_.checkState()))
            return true;

        return false;
    }
public:
    void update(uint64_t ts)
    {
        digitalRead(LOW_LEVEL_PIN);

        if (checkFailures())
            transition(States::Failure);

        stateop(dt);
    };

    void transition(State next)
    {
        auto from = state_;

        // transition functions
        if (from == States::Failure) //can't escape failure
            return;
        else if (from == States::Standby && next == States::Pump)
        {
            startPump_ = millis();
            digitalWrite(WATER_PUMP_PIN, FILL);
        }
        else if (from == States::Pump)
        {
            digitalWrite(WATER_PUMP_PIN, STOP);
        }
        state_ = dst;
    };

    void stateop(uint64_t ts)
    {
        switch(state_)
        {
            case States::Standby:
                return;
            case States::Pump:
                elapsedPump_ = ts;
                // update time pumped and keep pumping
                return;
            case States::Failure:
                // blink error light
                return;
        }
    };
};

unsigned int prev_time = 0;
unsigned int curr_time = 0;
unsigned int flow_time = 0;

void setup() {
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(RELAY_PIN,OUTPUT);
}
void loop() {
  unsigned int now = millis();
  flow_time = millis() - prev_time;
}
