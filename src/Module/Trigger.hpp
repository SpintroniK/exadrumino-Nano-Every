#pragma once

#include "../Memory/EEProm.hpp"

#include <avr/io.h>

#include <stdint.h>
#include <stdlib.h>

namespace Module
{
    using ParamType = uint8_t;
    using TriggerParam = Memory::EEPromValue<ParamType>;

    struct TriggerSettings
    {        

        TriggerSettings(const TriggerParam& threshold, const TriggerParam& scanTime, const TriggerParam& maskTime)
        : threshold{threshold}, scanTime{scanTime}, maskTime{maskTime}
        {

        }

        TriggerParam threshold;
        TriggerParam scanTime;
        TriggerParam maskTime;

    };
    
    
    class Trigger
    {

    public:

        Trigger() = default;
        ~Trigger() = default;

        Trigger(uint8_t thresh, uint8_t scan, uint8_t mask, ADC_MUXPOS_t channel)
        : threshold{thresh}, scanTime{scan}, maskTime{mask}, adcChannel{channel}
        {
            
        }

        Trigger(const Trigger& other)
        {
            threshold = other.threshold;
            scanTime = other.scanTime;
            maskTime = other.maskTime;
            adcChannel = other.adcChannel;
        }

        Trigger(Trigger&& other)
        {
            threshold = other.threshold;
            scanTime = other.scanTime;
            maskTime = other.maskTime;
            adcChannel = other.adcChannel;
        }

        Trigger& operator=(const Trigger& other)
        {
            threshold = other.threshold;
            scanTime = other.scanTime;
            maskTime = other.maskTime;
            adcChannel = other.adcChannel;

            return *this;
        }

        void Process(uint8_t value, uint8_t currentTime) noexcept
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
                    return;
                }
            }

            if(state == 1) // Scan
            {
                maxVelocity = velocity > maxVelocity ? velocity : maxVelocity;
                maxVelocity = maxVelocity >= 127 ? 127 : maxVelocity;

                const uint8_t delta = trigTime + scanTime - currentTime;
                if(static_cast<int8_t>(delta) <= 0)
                {
                    state = 2;
                    trigVelocity = maxVelocity;
                    return;
                }
            }

            if(state == 2)
            {
                const uint8_t delta = trigTime + maskTime - currentTime;
                if(static_cast<int8_t>(delta) <= 0)
                {
                    state = 0;
                    maxVelocity = 0;
                    return;
                }
            }

            return;
        }

        auto GetChannel() const noexcept
        {
            return adcChannel;
        }

        auto GetVelocity() const noexcept
        {
            return trigVelocity;
        }

        void Reset() noexcept
        {
            trigVelocity = 0;
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
        uint8_t trigVelocity{};

        // Parameters
        uint8_t threshold{};
        uint8_t scanTime{};
        uint8_t maskTime{};

        // ADC
        ADC_MUXPOS_t adcChannel{};
    };

} // namespace Module
