#pragma once

#include "../Util/Cpp.hpp"

#include <avr/io.h>

namespace Timing
{

    static const auto Tca0Addr = reinterpret_cast<uint16_t>(&TCA0);

    class TCASingle;
    class TCASplit;


    class TCAInterrupts
    {
    public:

        TCAInterrupts() = delete;
        ~TCAInterrupts() = delete;

    private:

        static void Overflow() __asm__("__vector_7") __attribute__((__signal__, __used__, __externally_visible__));
    };

    enum class TCASingleMode
    {
        Normal,
        Freq,
        SingleSlope,
        DSTop,
        DSBoth,
        DSBottom
    };

    enum class TCAClockDiv
    {
        Div1,
        Div2,
        Div4,
        Div8,
        Div16,
        Div64,
        Div256,
        Div1024
    };

    template <typename Mode>
    class TCA
    {

    public:

        TCA() = delete;
        ~TCA() = delete;

        static constexpr void ResetInterrupt()
        {
            tca().INTFLAGS |= TCA_SINGLE_OVF_bm;
        }

        static constexpr void SetSingleMode(TCASingleMode mode)
        {
            static_assert(Util::is_same_v<Mode, TCASingle>, "TCA should be in single mode.");

            switch(mode)
            {
                case TCASingleMode::Normal:         tca().CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc; break;
                case TCASingleMode::Freq:           tca().CTRLB = TCA_SINGLE_WGMODE_FRQ_gc; break;
                case TCASingleMode::SingleSlope:    tca().CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc; break;
                case TCASingleMode::DSTop:          tca().CTRLB = TCA_SINGLE_WGMODE_DSTOP_gc; break;
                case TCASingleMode::DSBoth:         tca().CTRLB = TCA_SINGLE_WGMODE_DSBOTH_gc; break;
                case TCASingleMode::DSBottom:       tca().CTRLB = TCA_SINGLE_WGMODE_DSBOTTOM_gc; break;
            }
        }

        template <TCAClockDiv div>
        static constexpr void SetClockDivider()
        {
            switch(div)
            {
                case TCAClockDiv::Div1: tca().CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc; break;
                case TCAClockDiv::Div2: tca().CTRLA |= TCA_SINGLE_CLKSEL_DIV2_gc; break;
                case TCAClockDiv::Div4: tca().CTRLA |= TCA_SINGLE_CLKSEL_DIV4_gc; break;
                case TCAClockDiv::Div8: tca().CTRLA |= TCA_SINGLE_CLKSEL_DIV8_gc; break;
                case TCAClockDiv::Div16: tca().CTRLA |= TCA_SINGLE_CLKSEL_DIV16_gc; break;
                case TCAClockDiv::Div64: tca().CTRLA |= TCA_SINGLE_CLKSEL_DIV64_gc; break;
                case TCAClockDiv::Div256: tca().CTRLA |= TCA_SINGLE_CLKSEL_DIV256_gc; break;
                case TCAClockDiv::Div1024: tca().CTRLA |= TCA_SINGLE_CLKSEL_DIV1024_gc; break;
            }
        }

        static constexpr void DisableEventCounting()
        {
            tca().EVCTRL &= ~TCA_SINGLE_CNTEI_bm;
        }

        static constexpr void SetPeriod(uint16_t period)
        {
            tca().PER = period;
        }

        static constexpr void Enable()
        {
            tca().CTRLA |= TCA_SINGLE_ENABLE_bm;
        }

    private:

        static constexpr auto& tca()
        {

            static_assert(Util::is_same_v<Mode, TCASingle> || Util::is_same_v<Mode, TCASplit>, 
                          "Error: TCA type doesn't exist.");

            if constexpr(Util::is_same_v<Mode, TCASingle>)
            {
                return reinterpret_cast<TCA_t*>(Tca0Addr)->SINGLE;
            }

            if constexpr(Util::is_same_v<Mode, TCASplit>)
            {
                return reinterpret_cast<TCA_t*>(Tca0Addr)->SPLIT;
            }
        }

    };
    
} // namespace Timing
