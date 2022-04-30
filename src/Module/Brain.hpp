#pragma once

#include "../Analog/Adc.hpp"
#include "../DigitalIO/Usart.hpp"
#include "Midi.hpp"
#include "Trigger.hpp"

namespace Module
{

    using Usart3 = DigitalIO::Usart<3>;
    using Adc0 = Analog::Adc<Analog::Adc0Addr, Analog::Adc8bitType>;

    static Usart3 usart{115'200};
    static Module::SerialMidi midi{usart};    

    class Brain
    {        

    public:
        Brain() = default;
        ~Brain() = default;


        inline void Process(uint8_t value, uint8_t currentTime)
        {
            const auto triggerVelocity = triggers[prevTriggerIndex].Process(value, currentTime);
            trigVelocities[prevTriggerIndex] = triggerVelocity > 0 ? triggerVelocity : 0;
        }

        inline auto NextChannel()
        {
            prevTriggerIndex = triggerIndex;
            triggerIndex = (triggerIndex + 1) % nbTriggers;
            return triggers[triggerIndex].GetChannel();
        }

        void SendMidiNotes()
        {

            for(uint8_t i = 0; i < nbTriggers; ++i)
            {
                const auto velocity = triggers[i].GetVelocity();
                if(velocity > 0)
                {
                    midi.NoteOn<MidiChannel>(triggers[i].GetMidiNote(), velocity);
                    triggers[i].Reset();
                }
            }
        }

    private:

        static constexpr uint8_t MidiChannel = 10;
        static constexpr uint8_t nbTriggers = 7;

        Trigger triggers[nbTriggers] = 
        {
            Trigger{3, 4, 40, 37, ADC_MUXPOS_t::ADC_MUXPOS_AIN3_gc}, 
            Trigger{3, 4, 40, 38, ADC_MUXPOS_t::ADC_MUXPOS_AIN2_gc},
            Trigger{3, 4, 40, 39, ADC_MUXPOS_t::ADC_MUXPOS_AIN1_gc},
            Trigger{3, 4, 40, 41, ADC_MUXPOS_t::ADC_MUXPOS_AIN12_gc},
            Trigger{3, 4, 40, 36, ADC_MUXPOS_t::ADC_MUXPOS_AIN13_gc},
            Trigger{3, 4, 40, 42, ADC_MUXPOS_t::ADC_MUXPOS_AIN4_gc},
            Trigger{3, 4, 40, 43, ADC_MUXPOS_t::ADC_MUXPOS_AIN5_gc},
        };

        uint8_t trigVelocities[nbTriggers]{};
        uint8_t triggerIndex{1};
        uint8_t prevTriggerIndex{0};
    
    };
    
} // namespace Module
