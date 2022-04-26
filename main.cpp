// #include <avr/iom4809.h>

#include "src/Module/Midi.hpp"
#include "src/Module/Trigger.hpp"
#include "src/Nano.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#include <stdio.h>
#include <stdlib.h>

using Led = DigitalIO::Pin<DigitalIO::PORTE_ADDR, PIN2_bm>;
using Usart3 = DigitalIO::Usart<3>;
using Adc0 = Analog::Adc<Analog::Adc0Addr, Analog::Adc8bitType>;
using Tca = Timing::TCA<Timing::TCASingle>;
using Tcb = Timing::TCB<0>;


Module::Trigger t{2, 4, 40};
Timing::Counter<uint8_t> clock{};

uint8_t value{};
volatile uint8_t vel{};

void Analog::AdcInterrupts::ResReady()
{
    Adc0::ResetInterrupt();

    value = t.Process(ADC0.RES, clock.GetCount());
    if(value > 0)
    {
        // Led::SetHigh();
        vel = value;
    }
    // else
    // {
    //     Led::SetLow();
    // }

}


void Timing::TCBInterrupts::TCB0Overflow()
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
    Module::SerialMidi midi{usart};


    // PD3 => AIN3
    PORTD.PIN3CTRL &= ~PORT_ISC_gm; 
    PORTD.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;    // Disable digital input
    PORTD.PIN3CTRL &= ~PORT_PULLUPEN_bm;            // Disable pull-up

    constexpr uint8_t EVSYS_GENERATOR_TCA0_OVF_gc = 0x80 << 0; // Should already be defined, but it's not...
    Event::EventSystem::Connect<0>(EVSYS_GENERATOR_TCA0_OVF_gc, EVSYS.USERADC0);

    // Configure ADC
    Adc0::EnableInterrupts();
    Adc0::EnableEvents();
    Adc0::SelectChannel(ADC_MUXPOS_AIN2_gc);
    Adc0::SetDivider<8>();
    Adc0::SetReference(Analog::Vref::External);
    Adc0::Enable();

    // Configure on-board LED
    Led::ConfigureAsOutput();

    // Configure TCA
    Tca::SetSingleMode(Timing::TCASingleMode::Normal);
    Tca::DisableEventCounting();
    Tca::SetPeriod(11);

    //TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;// | TCA_SINGLE_CMP0_bm;
    
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm;

    Tcb::EnableInterrupts();
    TCB0.CCMP = 999; 
    TCB0.CTRLA |= TCB_CLKSEL_CLKTCA_gc | TCB_ENABLE_bm;


    while(1)
    {
        if(vel >= 1)
        {
            midi.SendNote<0>(38, vel > 127 ? 127 : vel);

            vel = 0;
        }
    }

    return EXIT_SUCCESS;
}

