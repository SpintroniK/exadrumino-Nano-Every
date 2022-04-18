#pragma once

#include <avr/io.h>

#include "../IO/Pin.hpp"
#include "../IO/Port.hpp"

namespace Analog
{

    static const auto ADC0_ADDR = reinterpret_cast<uint16_t>(&ADC0);

    enum class Vref
    {
        External,
        Internal, 
        Vdd,
    };

    template <uint16_t addr>
    class Adc
    {
        
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
            // switch (div)
            // {
            //     case 2:   adc().CTRLC |= ADC_PRESC_DIV2_gc; break;
            //     case 4:   adc().CTRLC |= ADC_PRESC_DIV4_gc; break;
                //case 8:   
                adc().CTRLC = ADC_PRESC_DIV8_gc | ADC_REFSEL_VREFA_gc; //break;
                // case 16:  adc().CTRLC |= ADC_PRESC_DIV16_gc; break;
                // case 32:  adc().CTRLC |= ADC_PRESC_DIV32_gc; break;
                // case 64:  adc().CTRLC |= ADC_PRESC_DIV64_gc; break;
                // case 128: adc().CTRLC |= ADC_PRESC_DIV128_gc; break;
                // case 256: adc().CTRLC |= ADC_PRESC_DIV256_gc; break;
            //     default: static_assert(div % 2 == 0 && div >= 2 && div <= 256, "Invalid ADC prescaler.");
            // }
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

        static constexpr void SelectChannel()
        {

        }

        static constexpr void Enable()
        {
            ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc; // TODO
        }

        static constexpr void StartConversion()
        {
            ADC0.COMMAND = ADC_STCONV_bm;
        }

        template <typename T>
        static constexpr T ReadValue()
        {
            ADC0.INTFLAGS = ADC_RESRDY_bm;
            return static_cast<T>(ADC0.RES);
        }

        static constexpr bool ConversionDone()
        {
            return ADC0.INTFLAGS & ADC_RESRDY_bm;
        }

        static constexpr auto& adc()
        {
            return *reinterpret_cast<ADC_t*>(addr);
        }

    private:

    };
    
} // namespace Analog
