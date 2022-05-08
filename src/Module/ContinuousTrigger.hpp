#pragma once

#include <avr/io.h>

namespace Module
{
    
    class ContinuousTrigger
    {        
    public:
        ContinuousTrigger() = delete;
        ~ContinuousTrigger() = default;

        ContinuousTrigger(ADC_MUXPOS_t channel, uint8_t tScan)
        : adcChannel{channel}, scanTime{tScan}
        {

        }

        void Process(uint8_t value, uint8_t currentTime)
        {


            if(value < threshold && state == 0)
            {
                state = 1;
                trigTime = currentTime;
                trigValue = value;
            }
            
            if(value >= threshold && state > 1)
            {
                state = 0;
                trigTime = 0;
                trigValue = 0;
            }

            if(state == 1)
            {
                const uint8_t delta = trigTime + scanTime - currentTime;

                if(static_cast<int8_t>(delta) < 0)
                {
                    state = 2;
                    if(value < trigValue - velocityThresh)
                    {
                        const uint8_t velocity = trigValue - value;
                        trigVelocity = velocity > 127 ? 127 : velocity;
                        state = 3;
                    }
                }
            }

            const uint8_t sampledValue = value >> 5;

            if(sampledValue != prevCCValue)
            {
                controlChange = (sampledValue << 4) - 16;
            }
            else
            {
                controlChange = 0xff;
            }
            
            prevCCValue = sampledValue;
        }

        void Reset() noexcept
        {
            trigVelocity = 0;
        }

        auto GetControlChange() const noexcept
        {
            return controlChange;
        }

        auto GetChannel() const noexcept
        {
            return adcChannel;
        }

        auto GetCurrentState() const noexcept
        {
            return prevCCValue < 6;
        }

        auto GetTrigVelocity() const noexcept
        {
            return trigVelocity;
        }

    private:

        ADC_MUXPOS_t adcChannel{};

        uint8_t prevCCValue{};
        uint8_t controlChange{};

        uint8_t threshold{160};
        uint8_t scanTime{50};
        uint8_t velocityThresh{80};
        uint8_t trigVelocity{0};

        uint8_t state{0}; // 0 => rest / 1 => trig
        int8_t trigTime{};
        uint8_t trigValue{};

    };

} // namespace Module
