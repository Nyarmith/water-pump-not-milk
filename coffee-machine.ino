#include <SoftwareSerial.h>
#include "coffee-pins.hh"


template<uint8_t pin_, uint64_t debounceTime_, bool failOpen_>
class LevelSensor
{
public:

    bool checkState(uint64_t curTime)
    {
        bool measuredState = digitalRead(pin_);
        measuredState = failOpen_ ? !measuredState : measuredState;
        
        if (measuredState == curState_)
        {
            transitionTime_ = curTime;
        }

        if (curTime - transitionTime_ > debounceTime_)
        {
            transitionTime_ = curTime; 
            curState_ = measuredState;
        }
        
        return curState_;
    }

private:
    uint64_t transitionTime_;
    bool curState_;
};


enum class States { Standby, Pump, Failure };

class StateMachine
{
private:
    States current_{States::Standby};

    static constexpr uint64_t maxFillTime_{3*60*static_cast<uint64_t>(1000)};
    static constexpr uint64_t minEmptyTime_{5*60*static_cast<uint64_t>(1000)};
    uint64_t pumpTransitionOnTime_{0}; 
    uint64_t pumpTransitionOffTime_{minEmptyTime_}; 

    LevelSensor<LOW_LEVEL_PIN, 100, true> lowSensor_{};
    LevelSensor<HIGH_LEVEL_PIN, 100, true> highSensor_{};
    LevelSensor<FAILSAFE_LEVEL_PIN, 100, true> overflowSensor_{};


    bool checkFailures(uint64_t curTime)
    {
        // how long we've been pumping water
        if (curTime - pumpTransitionOnTime_ > maxFillTime_ and current_ == States::Pump)
            return true;

        // check that the tank didn't empty faster than should be possible
        // may signal bad sensor or leaking tank
        if (curTime - pumpTransitionOffTime_ < minEmptyTime_ and current_ == States::Pump)
            return true;


        // checking the sanity of our switch state

        // bad state: any time the sensors above see something the sensors below do
        // or the overflow sensor is on
        if (overflowSensor_.checkState(curTime) || (highSensor_.checkState(curTime) && !lowSensor_.checkState(curTime)))
            return true;

        return false;
    }
    
    void transition(States next, uint64_t curTime)
    {
        // on certain transitions we enable/disable stuff
        auto prev = current_;

        // transition functions
        if (prev == States::Failure) //can't escape failure
            return;
        else if (prev == States::Standby && next == States::Pump)
        {
            pumpTransitionOnTime_ = curTime;
            digitalWrite(WATER_PUMP_PIN, FILL);
        }
        else if (prev == States::Pump)
        {
            pumpTransitionOffTime_ = curTime;
            digitalWrite(WATER_PUMP_PIN, STOP);
        }
        current_ = next;
    }

    void stateop(uint64_t curTime)
    {
        switch(current_)
        {
            case States::Standby:
                if (!lowSensor_.checkState(curTime))
                {
                    // if the lowSensor is no longer on, pump
                    transition(States::Pump, curTime);
                }
                return;
            case States::Pump:
                if (highSensor_.checkState(curTime))
                {
                    // high sensor is on, stop pumping
                    transition(States::Standby, curTime);
                }
                // keep pumping,
                return;
            case States::Failure:
                // no possible transition out of failure state
                digitalWrite(WATER_PUMP_PIN, STOP);
                // blink error light
                return;
        }
    }

public:
    void update(uint64_t curTime)
    {
        if (checkFailures(curTime))
            transition(States::Failure, curTime);

        stateop(curTime);
    }
};

StateMachine pump;

void setup() {
  pinMode(LOW_LEVEL_PIN, INPUT_PULLUP);
  pinMode(HIGH_LEVEL_PIN, INPUT_PULLUP);
  pinMode(FAILSAFE_LEVEL_PIN, INPUT_PULLUP);
  pinMode(WATER_PUMP_PIN,OUTPUT);
  Serial.begin(9600);
}

void loop() {
    pump.update(millis());
}
