#pragma once

#include <avr/io.h>

#include "../DigitalIO/Pin.hpp"
#include "../DigitalIO/Port.hpp"
#include "../Util/Cpp.hpp"

namespace Analog
{

    static const auto ADC0_ADDR = reinterpret_cast<uint16_t>(&ADC0);

    using Adc10bitType = uint16_t;
    using Adc8bitType = uint8_t;

    class AdcInterrupts
    {
    public:

        AdcInterrupts() = delete;
        ~AdcInterrupts() = delete;

    private:

        static void ResReady() __asm__("__vector_22") __attribute__((__signal__, __used__, __externally_visible__));    
    };

    enum class Vref
    {
        External,
        Internal, 
        Vdd,
    };

    template <uint16_t addr, typename ValueType>
    class Adc
    {

        friend class AdcInterrupts;

    public:

        Adc() = delete;
        ~Adc() = delete;

        static constexpr void EnableInterrupts()
        {
            adc().INTCTRL |= ADC_RESRDY_bm;
        }

        static constexpr void EnableEvents()
        {
            adc().EVCTRL |= ADC_STARTEI_bm;
        }

        template <uint16_t div>
        static constexpr void SetDivider()
        {
            switch (div)
            {
                case 2:   adc().CTRLC |= ADC_PRESC_DIV2_gc; break;
                case 4:   adc().CTRLC |= ADC_PRESC_DIV4_gc; break;
                case 8:   adc().CTRLC |= ADC_PRESC_DIV8_gc; break;
                case 16:  adc().CTRLC |= ADC_PRESC_DIV16_gc; break;
                case 32:  adc().CTRLC |= ADC_PRESC_DIV32_gc; break;
                case 64:  adc().CTRLC |= ADC_PRESC_DIV64_gc; break;
                case 128: adc().CTRLC |= ADC_PRESC_DIV128_gc; break;
                case 256: adc().CTRLC |= ADC_PRESC_DIV256_gc; break;
                default: static_assert(div % 2 == 0 && div >= 2 && div <= 256, "Invalid ADC prescaler.");
            }
        }

        static constexpr void SetReference(Vref vref)
        {
            switch (vref)
            {
                case Vref::External: adc().CTRLC |= ADC_REFSEL_VREFA_gc; break;
                case Vref::Internal: adc().CTRLC |= ADC_REFSEL_INTREF_gc; break;
                case Vref::Vdd: adc().CTRLC |= ADC_REFSEL_VDDREF_gc; break;
            }
        }

        static constexpr void SelectChannel(ADC_MUXPOS_t chan)
        {
            channel = chan;
            ADC0.MUXPOS = channel;
        }

        static constexpr void Enable()
        {
            if constexpr(Util::is_same<ValueType, Adc8bitType>::value)
            {
                ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_8BIT_gc;
            }
            
            if constexpr(Util::is_same<ValueType, Adc10bitType>::value)
            {
                ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc;
            }
        }

        static constexpr void StartConversion()
        {
            ADC0.COMMAND = ADC_STCONV_bm;
        }


        static constexpr bool ConversionDone()
        {
            return ADC0.INTFLAGS & ADC_RESRDY_bm;
        }

        static constexpr auto& adc()
        {
            return *reinterpret_cast<ADC_t*>(addr);
        }

        static constexpr ValueType GetValue()
        {
            return value;
        }


    private:

        inline static ValueType value = 0;
        inline static ADC_MUXPOS_t channel = ADC_MUXPOS_t::ADC_MUXPOS_AIN0_gc;

    };
    
} // namespace Analog
