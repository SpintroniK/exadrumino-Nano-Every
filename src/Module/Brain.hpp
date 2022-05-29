#pragma once

#include "../Analog/Adc.hpp"
#include "../DigitalIO/Usart.hpp"
#include "Instruments/HiHat.hpp"
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
    inline constexpr auto hiHatCtrlChannel = ADC_MUXPOS_t::ADC_MUXPOS_AIN0_gc;
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

    inline constexpr uint8_t iKick = 0;
    inline constexpr uint8_t iSnare = 1;
    inline constexpr uint8_t iCrash = 2;
    inline constexpr uint8_t iTom = 3;
    inline constexpr uint8_t iFloorTom = 4;
    inline constexpr uint8_t iRide = 5;

    using TriggerParam = Memory::EEPromValue<ParamType>;

    class Brain
    {        

    public:
        Brain() = default;
        ~Brain() = default;


        template <uint8_t i>
        void SetPadTriggerSettings(uint8_t threshold, uint8_t scanTime, uint8_t maskTime)
        {
            static_assert(i < nbPads, "Error Pad index out of range.");
            
            pads[i].SetTrigger({threshold, scanTime, maskTime, channels[i]});
        }

        inline void Process(uint8_t value, uint8_t currentTime)
        {
            if(prevTriggerIndex == hiHatIndex)
            {
                hihat.ProcessTrigger(value, currentTime);
                return;
            }

            if(prevTriggerIndex == hiHatCtrlIndex)
            {
                hihat.ProcessController(value, currentTime);
                return;
            }

            pads[prevTriggerIndex].Process(value, currentTime);
        }

        inline auto NextChannel()
        {           

            prevTriggerIndex = triggerIndex;
            triggerIndex = (triggerIndex + 1) % nbInstruments;

            if(triggerIndex == hiHatIndex)
            {
                return hihat.GetTriggerChannel();
            }

            if(triggerIndex == hiHatCtrlIndex)
            {
                return hihat.GetControllerChannel();
            }

            return pads[triggerIndex].GetChannel();
        }

        void SendMidiNotes()
        {

            // HiHat
            const auto hiHatVelocity = hihat.GetVelocity();

            if(hiHatVelocity > 0)
            {
                //midi.NoteOn<MidiChannel>(hihat.GetMidiNote(), hiHatVelocity);
                hihat.Reset();
            }

            // HiHat controller
            const auto controlChange = hihat.GetControlChange();

            if(controlChange != 0xff)
            {
                midi.ControlChange<MidiChannel, 4>(controlChange);
            }

            // HiHat controller trig
            const auto pedalVelocity = hihat.GetPedalVelocity();

            if(pedalVelocity > 0)
            {
                //midi.NoteOn<MidiChannel>(hihat.GetPedalNote(), pedalVelocity);
                hihat.ResetPedal();
            }

            // Pads
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

        static constexpr uint8_t MidiChannel = 9;
        static constexpr uint8_t nbPads = 6;

        static constexpr uint8_t hiHatIndex = nbPads;
        static constexpr uint8_t hiHatCtrlIndex = hiHatIndex + 1;
        static constexpr uint8_t nbInstruments = hiHatCtrlIndex + 1;


        static constexpr decltype(kickChannel) channels[nbPads] = 
        {
            kickChannel, 
            snareChannel, 
            crashChannel, 
            tomTomChannel, 
            floorTomChannel, 
            rideChannel
        };


        Pad pads[nbPads] = 
        {
            Pad{kickNote}, 
            Pad{snareNote},
            Pad{crashNote},
            Pad{tomTomNote},
            Pad{floorTomNote},
            Pad{rideNote},
        };

        HiHat hihat
        { 
            {hiHatCtrlChannel, 50, 160, 80}, 42, 42, 44
        };

        uint8_t triggerIndex{1};
        uint8_t prevTriggerIndex{0};
    
    };
    
} // namespace Module
