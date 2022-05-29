#pragma once

#include "Pin.hpp"
#include "Port.hpp"

#include <avr/io.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace DigitalIO
{
    class UsartInterrupts
    {
    public:

        UsartInterrupts() = delete;
        ~UsartInterrupts() = delete;

    private:

        static void ReceivedWord() __asm__("__vector_37") __attribute__((__signal__, __used__, __externally_visible__));
    };

    template <uint8_t N>
    class Usart
    {
    public:

    constexpr explicit Usart(uint32_t br)
    {
        switch(N)
        {
            // TODO: configure Usart 0 to 2
            case 3: 
            {    
                PORTMUX.USARTROUTEA |= PORTMUX_USART30_bm;
                using Usart3InputPin = Pin<PORTB_ADDR, PIN5_bm>;
                using Usart3OutputPin = Pin<PORTB_ADDR, PIN4_bm>;

                Usart3InputPin::ConfigureAsInput();
                Usart3OutputPin::ConfigureAsOutput();
                
                break;
            }
            default: static_assert(N <= 3, "Invalid USART number."); break;
        }

        usart().BAUD = BaudRate(br);
        usart().CTRLB |= USART_RXEN_bm | USART_TXEN_bm; 
        usart().CTRLA |= USART_RXCIE_bm;
        usart().CTRLC |= USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc;
        
    }

    constexpr void SendByte(uint8_t c)
    {
        while(!(usart().STATUS & USART_DREIF_bm))
        {
            ;
        }        
        usart().TXDATAL = c;
    }

    constexpr uint8_t ReadByte() const
    {
        return usart().RXDATAL;
    }

    constexpr void ResetInterrupt()
    {
        usart().STATUS |= USART_RXCIF_bm;
    }

    // constexpr void SendString(char* str)
    // {
    //     for(size_t i = 0; i < strlen(str); i++)
    //     {
    //         SendByte(str[i]);
    //     }
    // }

    static constexpr uint16_t BaudRate(uint32_t baudRate)
    {
        return static_cast<uint16_t>(static_cast<float>((F_CPU * 64 / (16 * static_cast<float>(baudRate))) + 0.5));
    }

    static constexpr USART_t& usart()
    {
        switch(N)
        {
            case 0: return USART0;
            case 1: return USART1;
            case 2: return USART2;
            case 3: return USART3;
            default: static_assert(N <= 3, "Invalid USART number.");
        }
    }

    private:

    };

} // namespace IO

