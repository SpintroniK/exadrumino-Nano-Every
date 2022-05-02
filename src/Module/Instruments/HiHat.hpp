#pragma once

#include "../ContinuousTrigger.hpp"
#include "../Instrument.hpp"
#include "../Trigger.hpp"

namespace Module
{
    
    class HiHat : public Instrument
    {

    public:
        HiHat(Trigger&& t, ContinuousTrigger&& ct, uint8_t note)
        : trigger{static_cast<Trigger&&>(t)}, controller{static_cast<ContinuousTrigger&&>(ct)} ,midiNote{note}
        {

        }

        ~HiHat() = default;

        void Process(uint8_t value, uint8_t currentTime) noexcept
        {

        }

    private:

        Trigger trigger;
        ContinuousTrigger controller;
        uint8_t midiNote;
    };
    
} // namespace Modul
