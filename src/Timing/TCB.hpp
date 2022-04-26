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
    
    enum class TCBClock
    {
        Div1,
        Div2,
        TCA
    };

    
    template <uint8_t nb>
    class TCB
    {

    public:
        TCB() = delete;
        ~TCB() = delete;

        static constexpr void EnableInterrupts()
        {
            tcb().INTCTRL =  TCB_CAPT_bm;
        }

        static constexpr void SetCompareOrCapture(uint16_t value)
        {
            tcb().CCMP = value;
        }

        static constexpr void Enable()
        {
            tcb().CTRLA |= TCB_ENABLE_bm;
        }

        template <TCBClock clock>
        static constexpr void SetClock()
        {
            switch(clock)
            {
                case TCBClock::Div1: tcb().CTRLA |= TCB_CLKSEL_CLKDIV1_gc; break;
                case TCBClock::Div2: tcb().CTRLA |= TCB_CLKSEL_CLKDIV2_gc; break;
                case TCBClock::TCA: tcb().CTRLA |= TCB_CLKSEL_CLKTCA_gc; break;
            }
        }

    private:

        static constexpr auto& tcb()
        {
            switch(nb)
            {
                case 0: return TCB0;
                case 1: return TCB1;
                case 2: return TCB2;
                case 3: return TCB3;
                default: static_assert(nb <= 3, "Wrong TCB number.");
            }
        }

    };
    
    


} // namespace Timing
