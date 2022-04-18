// #include <avr/iom4809.h>

#include "src/Analog/Adc.hpp"
#include "src/IO/Usart.hpp"
#include "src/IO/Pin.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <stdlib.h>

using namespace IO;
using namespace Analog;

using Led = Pin<PORTE_ADDR, PIN2_bm>;
using Adc0 = Adc<ADC0_ADDR>;

int main()
{

    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00); // Disable prescaler

    sei();

    Usart<3> usart{115'200};


    // PD3 => AIN3
    PORTD.PIN3CTRL &= ~PORT_ISC_gm; 
    PORTD.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;    // Disable digital input
    PORTD.PIN3CTRL &= ~PORT_PULLUPEN_bm;            // Disable pull-up



    Adc0::EnableInterrupts();
    Adc0::EnableEvents();

    ADC0.MUXPOS  = ADC_MUXPOS_AIN3_gc;

    // ADC0.CTRLC = ADC_PRESC_DIV8_gc      /* CLK_PER divided by 4 */
    //            | ADC_REFSEL_VREFA_gc;  /* Internal reference */


    Adc0::SetDivider<8>();
    Adc0::SetReference(Vref::External);
    
    // ADC0.CTRLA = ADC_ENABLE_bm          /* ADC Enable: enabled */
    //            | ADC_RESSEL_10BIT_gc;   /* 10-bit mode */
    

    Adc0::Enable();


    Led::ConfigureAsOutput();


    while(1)
    {

        Adc0::StartConversion();

        Led::Toggle();
        while(!Adc0::ConversionDone())
        {
            ;
        }

        const auto value = Adc0::ReadValue<uint16_t>();

        char str[32];
        sprintf(str, "%d\n", value);

        usart.SendString(str);

    }

    return EXIT_SUCCESS;
}

