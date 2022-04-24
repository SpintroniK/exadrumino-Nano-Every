// #include <avr/iom4809.h>

#include "src/Module/Trigger.hpp"
#include "src/Nano.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#include <stdio.h>
#include <stdlib.h>

using namespace Analog;
using namespace DigitalIO;
using namespace Event;
using namespace Timing;

using Led = Pin<PORTE_ADDR, PIN2_bm>;

using Usart3 = Usart<3>;
using Adc0 = Adc<Adc0Addr, Adc8bitType>;
using Tca = TCA<TCASingle>;
using Tcb = TCB<0>;


Module::Trigger t{2, 4, 50};
Counter<uint8_t> clock{};

uint8_t value{};

void AdcInterrupts::ResReady()
{
    Adc0::ResetInterrupt();

    value = t.Process(ADC0.RES, clock.GetCount());
    if(value == 2)
    {
        Led::SetHigh();
    }
    else
    {
        Led::SetLow();
    }

}


void TCBInterrupts::TCB0Overflow()
{
    TCB0.INTFLAGS |= TCB_CAPT_bm;

    clock.Increment();
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

    constexpr uint8_t EVSYS_GENERATOR_TCA0_OVF_gc = 0x80 << 0; // Should already be defined, but it's not...
    EventSystem::Connect<0>(EVSYS_GENERATOR_TCA0_OVF_gc, EVSYS.USERADC0);

    // Configure ADC
    Adc0::EnableInterrupts();
    Adc0::EnableEvents();
    Adc0::SelectChannel(ADC_MUXPOS_AIN2_gc);
    Adc0::SetDivider<8>();
    Adc0::SetReference(Vref::External);
    Adc0::Enable();

    // Configure on-board LED
    Led::ConfigureAsOutput();

    // Configure TCA
    Tca::SetSingleMode(TCASingleMode::Normal);
    Tca::DisableEventCounting();
    Tca::SetPeriod(11);

    //TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;// | TCA_SINGLE_CMP0_bm;
    
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm;

    Tcb::EnableInterrupts();
    TCB0.CCMP = 999; 
    TCB0.CTRLA |= TCB_CLKSEL_CLKTCA_gc | TCB_ENABLE_bm;



    while(1)
    {
        //if(value >= 1)
        // {
        //     char str[32];
        //     sprintf(str, "%d\n", value);

        //     usart.SendString(str);
        // }
        ;
    }

    return EXIT_SUCCESS;
}

