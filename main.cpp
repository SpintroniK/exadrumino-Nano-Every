#include "src/Usart.hpp"

// #include <avr/iom4809.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdlib.h>

int main()
{

    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00); // Disable prescaler

    sei();

    Usart<3> usart{115'200};

    while(1)
    {
        usart.SendChar('a');
    }

    return EXIT_SUCCESS;
}

