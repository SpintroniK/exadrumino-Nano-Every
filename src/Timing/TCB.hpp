#pragma once

#include <stdint.h>

namespace Timing
{

    class TCBInterrupts
    {

    public:
        TCBInterrupts() = delete;
        ~TCBInterrupts() = delete;

    private:

        static void TCB0Overflow() __asm__("__vector_12") __attribute__((__signal__, __used__, __externally_visible__));
        static void TCB1Overflow() __asm__("__vector_13") __attribute__((__signal__, __used__, __externally_visible__));

    };
    

    
    template <uint8_t nb>
    class TCB
    {

    public:
        TCB() = delete;
        ~TCB() = delete;

        static constexpr void EnableInterrupts()
        {
            switch(nb)
            {
                case 0: TCB0.INTCTRL =  TCB_CAPT_bm; break;
                default: static_assert(nb <= 3, "Wrong TCB number.");
            }
        }

    private:

    };
    
    


} // namespace Timing
