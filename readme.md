EZBL- PIC 24 boot loader ( PIC24HJ256GP610A)

In this document i am trying to explain the steps it takes to use the EZBL
boot loader from Microchip.

The sources are only two main files that blink leds , all sources can be
download from the ezbl-v2.04.

Use MPLAB-X 4.05 to compile the ex_boot_uart boot loader , i did not managed
To get it compiled with 4.15.

all other apps are compiled fine with 4.15.

The EZBL boot loader integration is explain in the help file exercise but the document here explain steps by step how to get everything work.

The EZBL example works with PIC24F where here I am using PIC24H
The change of the device also help to understands how to use the bootloader .S file correct and use it in your project.

When you compile the ex_boot_uart.s  it will generate a .S file.
This file need to be copied to your application into the ezbl_integration.

If you copy the boot loader with the led app without the .S file when you change a device chip id it will not work.

Finally , to get your own app wrapped as a boot loader integration , it is better to start with the 
ex_app_led_blink
and add your project code into that project , it is better than using the opposite direction
from your app to the bootloader.


the ezbl_c_loader is a c code , console app that load a bl2 file just like
the java code is doing.
