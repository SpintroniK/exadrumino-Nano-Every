// #include <avr/iom4809.h>

#include "src/Module/Brain.hpp"
#include "src/Nano.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <stdlib.h>

using Led = DigitalIO::Pin<DigitalIO::PORTE_ADDR, PIN2_bm>;
using Adc0 = Analog::Adc<Analog::Adc0Addr, Analog::Adc8bitType>;
using Tca = Timing::TCA<Timing::TCASingle>;
using Tcb = Timing::TCB<0>;


Module::Brain brain{};

Timing::Counter<uint8_t> clock{};


void Analog::AdcInterrupts::ResReady()
{
    // Led::SetHigh();
    Adc0::ResetInterrupt();
    brain.Process(ADC0.RES, clock.GetCount());
    Adc0::SelectChannel(brain.NextChannel());
    // Led::SetLow();
}

void Timing::TCBInterrupts::TCB0Overflow()
{
    Tcb::ResetInterrupt();
    clock.Increment();
}

int main()
{

    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00); // Disable prescaler

    sei();

    Event::EventSystem::Connect<0>(EVSYS_GENERATOR_TCA0_OVF_gc, EVSYS.USERADC0);

    // Configure ADC
    Adc0::EnableInterrupts();
    Adc0::EnableEvents();
    Adc0::SelectChannel(ADC_MUXPOS_AIN2_gc);
    Adc0::SetPrescaler<Analog::Prescaler::Div8>();
    Adc0::SetReference<Analog::Vref::External>();
    Adc0::Enable();

    // Configure on-board LED
    Led::ConfigureAsOutput();

    // Configure TCA
    Tca::SetSingleMode<Timing::TCASingleMode::Normal>();
    Tca::DisableEventCounting();
    Tca::SetPeriod(13);             // 14 µs

    
    Tca::SetClockDivider<Timing::TCAClockDiv::Div16>();
    Tca::Enable();

    Tcb::EnableInterrupts();
    Tcb::SetCompareOrCapture(999); // 1'000 µs =  1 ms
    Tcb::SetClock<Timing::TCBClock::TCA>();
    Tcb::Enable();


    for(;;)
    {
        brain.SendMidiNotes();
    }

    return EXIT_SUCCESS;
}

