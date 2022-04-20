//  #include <avr/iom4809.h>

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
using Adc0 = Adc<ADC0_ADDR, Adc8bitType>;


void AdcInterrupts::ResReady()
{
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    Adc0::value = ADC0.RES;
}


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

    Adc0::SetDivider<8>();
    Adc0::SetReference(Vref::External);

    Adc0::Enable();


    Led::ConfigureAsOutput();


    while(1)
    {

        Adc0::StartConversion();

        Led::Toggle();
        // while(!Adc0::ConversionDone())
        // {
        //     ;
        // }

        // const auto value = Adc0::ReadValue<uint16_t>();

        char str[32];
        sprintf(str, "%d\n", Adc0::GetValue());

        usart.SendString(str);

    }

    return EXIT_SUCCESS;
}

