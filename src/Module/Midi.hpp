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

        template <uint8_t channel, uint8_t control>
        inline void ControlChange(uint8_t value) const noexcept
        {
            static_assert(control <= 127, "Invalid control value.");
            usart_.SendByte(0xB0 | (channel & 0x0f));
            usart_.SendByte(control);
            usart_.SendByte(value);
        }


    private:

        Usart& usart_;

    };

} // namespace Module
