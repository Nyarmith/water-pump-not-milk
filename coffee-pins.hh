constexpr unsigned LOW_LEVEL_PIN        = 2;
constexpr unsigned HIGH_LEVEL_PIN       = 3;
constexpr unsigned FAILSAFE_LEVEL_PIN   = 4;
constexpr unsigned WATER_PUMP_PIN       = 5;

constexpr unsigned FILL = HIGH;
constexpr unsigned STOP = LOW;

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
    uint64_t transitionTime_{0};
    bool curState_{false};
    static constexpr debounceTime_ {250}; // millis
};
