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

    inline constexpr auto kickChannel = ADC_MUXPOS_t::ADC_MUXPOS_AIN3_gc;
    inline constexpr auto snareChannel = ADC_MUXPOS_t::ADC_MUXPOS_AIN2_gc;
    inline constexpr auto hiHatChannel = ADC_MUXPOS_t::ADC_MUXPOS_AIN1_gc;
    inline constexpr auto crashChannel = ADC_MUXPOS_t::ADC_MUXPOS_AIN12_gc;
    inline constexpr auto tomTomChannel = ADC_MUXPOS_t::ADC_MUXPOS_AIN13_gc;
    inline constexpr auto floorTomChannel = ADC_MUXPOS_t::ADC_MUXPOS_AIN4_gc;
    inline constexpr auto rideChannel = ADC_MUXPOS_t::ADC_MUXPOS_AIN5_gc;

    inline constexpr uint8_t kickNote = 36;
    inline constexpr uint8_t snareNote = 38;
    inline constexpr uint8_t hiHatNote = 42; // closed => 42, open => 46, semi-open => 52
    inline constexpr uint8_t crashNote = 49;
    inline constexpr uint8_t tomTomNote = 45;
    inline constexpr uint8_t floorTomNote = 41;
    inline constexpr uint8_t rideNote = 48;

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
            Trigger{3, 4, 40, kickNote, kickChannel}, 
            Trigger{3, 4, 40, snareNote, snareChannel},
            Trigger{6, 4, 40, hiHatNote, hiHatChannel},
            Trigger{3, 4, 40, crashNote, crashChannel},
            Trigger{3, 4, 40, tomTomNote, tomTomChannel},
            Trigger{3, 4, 40, floorTomNote, floorTomChannel},
            Trigger{3, 4, 40, rideNote, rideChannel},
        };

        uint8_t trigVelocities[nbTriggers]{};
        uint8_t triggerIndex{1};
        uint8_t prevTriggerIndex{0};
    
    };
    
} // namespace Module
