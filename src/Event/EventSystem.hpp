#pragma once

#include <avr/io.h>

#include <stdint.h>

namespace Event
{

    using Generator = uint8_t;
    using Event = register8_t;

    class EventSystem
    {
        
    public:
        EventSystem() = delete;
        ~EventSystem() = delete;

        template <uint8_t channel>
        static constexpr void Connect(Generator in, Event& out)
        {
            switch(channel)
            {
                case 0:
                {
                    EVSYS.CHANNEL0 = in;
                    out = EVSYS_CHANNEL_CHANNEL0_gc;
                    break;
                }

                case 1:
                {
                    EVSYS.CHANNEL1 = in;
                    out = EVSYS_CHANNEL_CHANNEL1_gc;
                    break;
                }

                default: static_assert(channel >= 0 && channel <= 8, ""); break;
            }
        }

    private:

    };
    
} // namespace Event
