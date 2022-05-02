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

    private:

        ADC_MUXPOS_t adcChannel{};

    };

} // namespace Module
