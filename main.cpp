// #include <avr/iom4809.h>

#include "src/IO/Usart.hpp"
#include "src/IO/Pin.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdlib.h>

using namespace IO;

using Led = Pin<PORTE_ADDR, PIN2_bm>;

int main()
{

    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00); // Disable prescaler

    sei();

    Usart<3> usart{115'200};


    Led::ConfigureAsOutput();

    while(1)
    {
        Led::Toggle();
        usart.SendChar('c');
    }

    return EXIT_SUCCESS;
}

