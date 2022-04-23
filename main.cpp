// #include <avr/iom4809.h>

#include "src/Analog/Adc.hpp"
#include "src/DigitalIO/Usart.hpp"
#include "src/DigitalIO/Pin.hpp"
#include "src/Timing/TCA.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#include <stdio.h>
#include <stdlib.h>

using namespace Analog;
using namespace DigitalIO;
using namespace Timing;

using Led = Pin<PORTE_ADDR, PIN2_bm>;

using Usart3 = Usart<3>;
using Adc0 = Adc<Adc0Addr, Adc8bitType>;
using Tca = TCA<TCASingle>;


void AdcInterrupts::ResReady()
{
    Adc0::ResetInterrupt();
    Adc0::value = ADC0.RES;

    Led::Toggle();
}

void TCAInterrupts::Overflow()
{
    TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_OVF_bm;
}


int main()
{

    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00); // Disable prescaler

    sei();

    // Configure USART
    Usart3 usart{115'200};


    // PD3 => AIN3
    PORTD.PIN3CTRL &= ~PORT_ISC_gm; 
    PORTD.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;    // Disable digital input
    PORTD.PIN3CTRL &= ~PORT_PULLUPEN_bm;            // Disable pull-up

    EVSYS.CHANNEL0 = 0x80<<0; //EVSYS_GENERATOR_TCA0_OVF_gc;
    EVSYS.USERADC0 = EVSYS_CHANNEL_CHANNEL0_gc;

    // Configure ADC
    Adc0::EnableInterrupts();
    Adc0::EnableEvents();
    Adc0::SelectChannel(ADC_MUXPOS_AIN3_gc);
    Adc0::SetDivider<8>();
    Adc0::SetReference(Vref::External);
    Adc0::Enable();

    // Configure on-board LED
    Led::ConfigureAsOutput();

    // Configure TCA
    Tca::SetSingleMode(TCASingleMode::Normal);
    Tca::DisableEventCounting();
    Tca::SetPeriod(15);

    TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;// | TCA_SINGLE_CMP0_bm;
    
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm;


    while(1)
    {

        char str[32];
        sprintf(str, "%d\n", Adc0::GetValue());

        usart.SendString(str);

    }

    return EXIT_SUCCESS;
}

