
#include <xc.h>

#define FCY     16000000                // Execution speed for __delay_us()/__delay_ms() macros
#include <libpic30.h>


int main(void)
{
       // Set RA<7:0> LED pins as GPIO outputs
    LATA  &= 0xFF00;
    TRISA &= 0xFF00;
     

    while(1)
    {
        LATA ^= 0x0055;
        __delay_ms(500);
    }



}
