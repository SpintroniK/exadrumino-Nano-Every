#pragma once

#include "../Instrument.hpp"
#include "../Trigger.hpp"

namespace Module
{
    class Pad : public Instrument
    {
        
    public:
        Pad(Trigger&& t, uint8_t note) 
        : trigger{static_cast<Trigger&&>(t)}, midiNote{note}
        {

        }

        ~Pad() = default;

        void Process(uint8_t value, uint8_t currentTime) noexcept
        {
            trigger.Process(value, currentTime);
        }

        auto GetChannel() const noexcept
        {
            return trigger.GetChannel();
        }

        auto GetVelocity() const noexcept
        {
            return trigger.GetVelocity();
        }

        void Reset() noexcept
        {
            trigger.Reset();
        }

        auto GetMidiNote() const noexcept
        {
            return midiNote;
        }

    private:

        Trigger trigger;
        uint8_t midiNote;

    };
    
} // namespace Module
