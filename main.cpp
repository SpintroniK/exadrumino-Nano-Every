#include "src/Module/Brain.hpp"
#include "src/Commands/Executor.hpp"
#include "src/Commands/Parser.hpp"
#include "src/Nano.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using Led = DigitalIO::Pin<DigitalIO::PORTE_ADDR, PIN2_bm>;
using Adc0 = Analog::Adc<Analog::Adc0Addr, Analog::Adc8bitType>;
using Tca = Timing::TCA<Timing::TCASingle>;
using Tcb = Timing::TCB<0>;

Module::Brain brain{};

uint8_t _kickThreshold EEMEM = 3;
uint8_t _kickScanTime  EEMEM = 4;
uint8_t _kickMaskTime EEMEM = 40;

auto kickThreshold = Memory::EEPromValue{&_kickThreshold};
// auto kickScanTime = Memory::EEPromValue{&_kickScanTime};
// auto kickMaskTime = Memory::EEPromValue{&_kickMaskTime};

// uint8_t kickThreshold = 3;
uint8_t kickScanTime = 4;
uint8_t kickMaskTime = 40;

//using TriggerParams = uint8_t*;//decltype(kickThreshold);

uint8_t* triggerParams[][3]
{
    {&_kickThreshold, &_kickScanTime, &_kickMaskTime}
};

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

Commands::Parser parser;

void DigitalIO::UsartInterrupts::ReceivedWord()
{
    Module::usart.ResetInterrupt();

    // Read incoming byte
    const auto b = Module::usart.ReadByte();

    parser.Parse(b);
    const auto command = parser.PopCommand();

    if(command)
    {
        const auto res = Commands::Executor::Run(*command, triggerParams);

        // eeprom_update_byte(res, command->type);
 
        // const auto value = eeprom_read_byte(res);
        if(res != 0)
        {
            kickThreshold = res;
        }
        const uint8_t value = kickThreshold;
    
        char str[32]{};
        ::sprintf(str, "\nCommand type: %d\n", value);
        for(uint8_t i = 0; i < ::strlen(str); ++i)
        {
            Module::usart.SendByte(str[i]);
        }
        // const uint8_t v = 3; //command->args.arg2;
        // kickThreshold = v;

        // brain.SetPadTriggerSettings<Module::iSnare>(kickThreshold, kickScanTime, kickMaskTime);
    }
}

int main()
{

    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00); // Disable prescaler

    // Module set-up

    brain.SetPadTriggerSettings<Module::iKick>(kickThreshold, kickScanTime, kickMaskTime);
    brain.SetPadTriggerSettings<Module::iSnare>(kickThreshold, kickScanTime, kickMaskTime);
    brain.SetPadTriggerSettings<Module::iCrash>(kickThreshold, kickScanTime, kickMaskTime);
    brain.SetPadTriggerSettings<Module::iTom>(kickThreshold, kickScanTime, kickMaskTime);
    brain.SetPadTriggerSettings<Module::iFloorTom>(kickThreshold, kickScanTime, kickMaskTime);
    brain.SetPadTriggerSettings<Module::iRide>(kickThreshold, kickScanTime, kickMaskTime);

    sei();

    Event::EventSystem::Connect<0>(EVSYS_GENERATOR_TCA0_OVF_gc, EVSYS.USERADC0);

    // Configure ADC
    Adc0::EnableInterrupts();
    Adc0::EnableEvents();
    Adc0::SelectChannel(ADC_MUXPOS_AIN0_gc);
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
