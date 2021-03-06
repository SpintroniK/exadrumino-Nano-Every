#pragma once

#include "../ContinuousTrigger.hpp"
#include "../Instrument.hpp"
#include "../Trigger.hpp"

namespace Module
{
    
    class HiHat : public Instrument
    {

    public:
        HiHat(Trigger&& t, ContinuousTrigger&& ct, uint8_t noteOpen, uint8_t noteClosed, uint8_t pedalNote)
        : trigger{static_cast<Trigger&&>(t)}, 
        controller{static_cast<ContinuousTrigger&&>(ct)}, 
        midiNoteOpen{noteOpen}, midiNoteClosed{noteClosed}, midiNotePedal{pedalNote}
        {

        }

        ~HiHat() = default;

        void ProcessTrigger(uint8_t value, uint8_t currentTime) noexcept
        {
            trigger.Process(value, currentTime);
        }

        void ProcessController(uint8_t value, uint8_t currentTime) noexcept
        {
            controller.Process(value, currentTime);
        }

        auto GetControlChange() const noexcept
        {
            return controller.GetControlChange();
        }

        auto GetMidiNote() const noexcept
        {
            return controller.GetCurrentState() == 0 ? midiNoteClosed : midiNoteOpen;
        }

        auto GetPedalNote() const noexcept
        {
            return midiNotePedal;
        }

        auto GetVelocity() const noexcept
        {
            return trigger.GetVelocity();
        }

        auto GetPedalVelocity() const noexcept
        {
            return controller.GetTrigVelocity();
        }

        void Reset() noexcept
        {
            trigger.Reset();
        }

        void ResetPedal() noexcept
        {
            controller.Reset();
        }

        auto GetTriggerChannel() const noexcept
        {
            return trigger.GetChannel();
        }

        auto GetControllerChannel() const noexcept
        {
            return controller.GetChannel();
        }

    private:

        Trigger trigger;
        ContinuousTrigger controller;
        uint8_t midiNoteOpen;
        uint8_t midiNoteClosed;
        uint8_t midiNotePedal;
    };
    
} // namespace Modul
