#pragma once


#include <avr/io.h>

#include <stdint.h>
#include <stdlib.h>

template<uint8_t N>
class Usart
{
public:

    Usart(uint32_t br)
    {
        switch(N)
        {
            case 0: usart = &USART0; break;
            case 1: usart = &USART1; break;
            case 2: usart = &USART2; break;
            case 3: 
            {
                usart = &USART3; 
    
                PORTMUX.USARTROUTEA |= PORTMUX_USART30_bm;

                PORTB.DIR &= ~PIN5_bm;
                PORTB.DIR |= PIN4_bm;
                break;
            }
            default: break;
        }

        usart->BAUD = Usart::BaudRate(br);
        usart->CTRLB |= USART_RXEN_bm | USART_TXEN_bm; 
        usart->CTRLC |= USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc;
    }

    void SendChar(char c)
    {
        while (!(usart->STATUS & USART_DREIF_bm))
        {
            ;
        }        
        usart->TXDATAL = c;
    }
    
    static constexpr uint16_t BaudRate(uint32_t baudRate)
    {
        return static_cast<uint16_t>((float)(F_CPU * 64 / (16 * (float)baudRate)) + 0.5);
    }

private:

    USART_t* usart{nullptr};
};
