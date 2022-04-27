#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace Module
{
    
    class Trigger
    {

    public:

        Trigger() = default;
        ~Trigger() = default;

        Trigger(uint8_t thresh, uint8_t scan, uint8_t mask, uint8_t note)
        : threshold{thresh}, scanTime{scan}, maskTime{mask}, midiNote{note}
        {
            
        }

        uint8_t Process(uint8_t value, uint8_t currentTime) noexcept
        {
            acc   -= prevX;
            prevX = static_cast<int16_t>(value) << 7;
            acc   += prevX;
            acc   -= prevY << 2;
            prevY = acc >> 7;

            const uint8_t velocity = ::abs(prevY);


            if(state == 0) // Idle
            {
                if(velocity >= threshold)
                {
                    trigTime = currentTime;
                    state = 1;
                    return 0;
                }

            }

            if(state == 1) // Scan
            {
                maxVelocity = velocity > maxVelocity ? velocity : maxVelocity;

                const uint8_t delta = trigTime + scanTime - currentTime;
                if(static_cast<int8_t>(delta) <= 0)
                {
                    const uint8_t vel = maxVelocity; // HACK
                    state = 2;
                    return maxVelocity;
                }
            }

            if(state == 2)
            {
                const uint8_t delta = trigTime + maskTime - currentTime;
                if(static_cast<int8_t>(delta) <= 0)
                {
                    state = 0;
                    maxVelocity = 0;
                    return 0;
                }
                
            }

            return 0;
        }

        auto GetVelocity() const noexcept
        {
            return maxVelocity;
        }

        auto GetMidiNote() const noexcept
        {
            return midiNote;
        }

        void SetTreshold(uint8_t t)
        {
            threshold = t;
        }

        void SetScanTime(uint8_t s)
        {
            scanTime = s;
        }

        void SetMaskTime(uint8_t m)
        {
            maskTime = m;
        }

    private:

        // Filter
        int16_t prevX{};
        int16_t prevY{};
        int16_t acc{};

        // State
        uint8_t state{0};
        uint8_t maxVelocity{};
        int8_t trigTime{};

        // Parameters
        uint8_t threshold{};
        uint8_t scanTime{};
        uint8_t maskTime{};

        // MIDI
        uint8_t midiNote{};
    };

} // namespace Module
