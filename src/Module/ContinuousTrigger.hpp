#pragma once

#include <avr/io.h>

namespace Module
{
    
    class ContinuousTrigger
    {        
    public:
        ContinuousTrigger() = delete;
        ~ContinuousTrigger() = default;

        ContinuousTrigger(ADC_MUXPOS_t channel)
        : adcChannel{channel}
        {

        }

        void Process(uint8_t value, uint8_t currentTime)
        {
            if(value > 200)
            {
                state = 1;
            }
            else
            {
                state = 0;
            }
            

            if(::abs(prevValue - value) > 2)
            {
                controlChange = value >> 1;
            }
            else
            {
                controlChange = 0;
            }

            prevValue = value;
        }

        auto GetControlChange() const noexcept
        {
            return controlChange;
        }

        auto GetChannel() const noexcept
        {
            return adcChannel;
        }

        auto GetState() const noexcept
        {
            return state;
        }

    private:

        ADC_MUXPOS_t adcChannel{};
        uint8_t state{0}; // 0 => closed / 1 => open
        uint8_t prevValue{};
        uint8_t controlChange{};

    };

} // namespace Module
