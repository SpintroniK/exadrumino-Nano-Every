#pragma once

#include "../DigitalIO/Usart.hpp"
#include "Midi.hpp"
#include "Trigger.hpp"

namespace Module
{

    using Usart3 = DigitalIO::Usart<3>;


    Usart3 usart{115'200};
    Module::SerialMidi midi{usart};

    class Brain
    {        

    public:
        Brain() = default;
        ~Brain() = default;


        inline void Process(uint8_t value, uint8_t currentTime)
        {
            const auto triggerVelocity = triggers[0].Process(value, currentTime);
            trigVelocities[0] = triggerVelocity > 0 ? triggerVelocity : 0;
        }

        void SendMidiNotes()
        {
            for(uint8_t i{}; i < nbTriggers; ++i)
            {
                if(trigVelocities[i] > 0)
                {
                    midi.NoteOn<MidiChannel>(triggers[0].GetMidiNote(), triggers[0].GetVelocity());
                    trigVelocities[i] = 0;
                }
            }
        }

    private:

        static constexpr uint8_t MidiChannel = 10;
        static constexpr uint8_t nbTriggers = 1;

        Trigger triggers[nbTriggers] = 
        {
            Trigger{2, 4, 40, 38}, 
        };

        volatile uint8_t trigVelocities[nbTriggers] = {0, };
        //uint8_t triggerIndex{};
    
    };
    
} // namespace Module
