#pragma once

#include "../DigitalIO/Usart.hpp"

namespace Module
{

    template <typename Usart>   
    class SerialMidi
    {

    public:

        SerialMidi() = delete;
        ~SerialMidi() = default;

        SerialMidi(Usart& usart) : usart_{usart}
        {

        }

        template <uint8_t channel>
        inline void NoteOn(uint8_t note, uint8_t velocity) const noexcept
        {
            usart_.SendByte(0x90 | (channel & 0x0f));
            usart_.SendByte(note);
            usart_.SendByte(velocity);
        }


    private:

        Usart& usart_;

    };

} // namespace Module
