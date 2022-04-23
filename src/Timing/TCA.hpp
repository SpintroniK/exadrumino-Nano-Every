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

            switch (mode)
            {
            case TCASingleMode::Normal:         tca().CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc; break;
            case TCASingleMode::Freq:           tca().CTRLB = TCA_SINGLE_WGMODE_FRQ_gc; break;
            case TCASingleMode::SingleSlope:    tca().CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc; break;
            case TCASingleMode::DSTop:          tca().CTRLB = TCA_SINGLE_WGMODE_DSTOP_gc; break;
            case TCASingleMode::DSBoth:         tca().CTRLB = TCA_SINGLE_WGMODE_DSBOTH_gc; break;
            case TCASingleMode::DSBottom:       tca().CTRLB = TCA_SINGLE_WGMODE_DSBOTTOM_gc; break;
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
