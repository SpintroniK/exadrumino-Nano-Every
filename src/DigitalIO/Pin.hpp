#pragma once

#include <avr/io.h>

#include <stdint.h>

namespace DigitalIO
{

    template <uint16_t portAddr, uint8_t mask>
    class Pin
    {
    public:
        Pin() = delete;
        ~Pin() = delete;

        static constexpr void ConfigureAsInput()
        {
            port().DIRSET &= ~mask;
        }

        static constexpr void ConfigureAsOutput()
        {
            port().DIRSET |= mask;
        }

        static constexpr void Toggle()
        {
            port().OUTTGL |= mask;
        }

        static constexpr auto& port()
        {
            return *reinterpret_cast<PORT_t*>(portAddr);
        }
    private:
    };
    
} // namespace IO
