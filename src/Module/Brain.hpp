#pragma once

#include "../Analog/Adc.hpp"
#include "../DigitalIO/Usart.hpp"
#include "Instruments/Pad.hpp"
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
            pads[prevTriggerIndex].Process(value, currentTime);
        }

        inline auto NextChannel()
        {
            prevTriggerIndex = triggerIndex;
            triggerIndex = (triggerIndex + 1) % nbPads;
            return pads[triggerIndex].GetChannel();
        }

        void SendMidiNotes()
        {

            for(uint8_t i = 0; i < nbPads; ++i)
            {
                const auto velocity = pads[i].GetVelocity();
                if(velocity > 0)
                {
                    midi.NoteOn<MidiChannel>(pads[i].GetMidiNote(), velocity);
                    pads[i].Reset();
                }
            }
        }

    private:

        static constexpr uint8_t MidiChannel = 10;
        static constexpr uint8_t nbPads = 7;

        Pad pads[nbPads] = 
        {
            Pad{{3, 4, 40, kickChannel}, kickNote}, 
            Pad{{3, 4, 40, snareChannel}, snareNote},
            Pad{{6, 4, 40, hiHatChannel}, hiHatNote},
            Pad{{3, 4, 40, crashChannel}, crashNote},
            Pad{{3, 4, 40, tomTomChannel}, tomTomNote},
            Pad{{3, 4, 40, floorTomChannel}, floorTomNote},
            Pad{{3, 4, 40, rideChannel}, rideNote},
        };

        uint8_t triggerIndex{1};
        uint8_t prevTriggerIndex{0};
    
    };
    
} // namespace Module
