/*
Copyright (c) 2012 Vishay GmbH, www.vishay.com
author: DS, version 1.2

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#define VERSION "\n Version: 1.2  01/2012\n"
#define MAIN5             // select MAIN1, MAIN2, MAIN3, MAIN4 or MAIN5.
                          //*Please note that MAIN1 and MAIN4 must be modified to be used with VCNL3020*(no ambient measurements) Eg.MAIN5  
#define BAUD    115200    // increase up to 921600 for high speed communication (depends on terminal programm and USB mode)

#include "mbed.h"
#include "VCNL40x0.h"

VCNL40x0 VCNL40x0_Device (p28, p27, VCNL40x0_ADDRESS);      // Define SDA, SCL pin and I2C address
DigitalOut mled0(LED1);                                     // LED #1
DigitalOut mled1(LED2);                                     // LED #2
DigitalOut mled2(LED3);                                     // LED #3
Serial pc(USBTX, USBRX);                                    // Tx, Rx USB transmission

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// main #1
// Read Proximity on demand and Ambient light on demand in endless loop
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
# ifdef MAIN1

int main() {
    unsigned char ID=0;
    unsigned char Current=0;
    unsigned int  ProxiValue=0;
    unsigned int  AmbiValue=0;

    pc.baud(BAUD);                                          // set USB speed (virtual COM port)

    // print information on screen
    pc.printf("\n\n VCNL4010/4020/3020 Proximity/Ambient Light Sensor");
    pc.printf("\n library tested with mbed LPC1768 (ARM Cortex-M3 core) on www.mbed.org");
    pc.printf(VERSION);
    pc.printf("\n Demonstration #1:");
    pc.printf("\n Read Proximity on demand and Ambient light on demand in endless loop");

    VCNL40x0_Device.ReadID (&ID);                           // Read VCNL40x0 product ID revision register
    pc.printf("\n\n Product ID Revision Register: %d", ID);

    VCNL40x0_Device.SetCurrent (20);                        // Set current to 200mA
    VCNL40x0_Device.ReadCurrent (&Current);                 // Read back IR LED current
    pc.printf("\n IR LED Current: %d\n\n", Current);

    wait_ms(3000);                                          // wait 3s (only for display)

// endless loop /////////////////////////////////////////////////////////////////////////////////////

    while (1) {
        mled0 = 1;                                          // LED on
        VCNL40x0_Device.ReadProxiOnDemand (&ProxiValue);    // read prox value on demand
        VCNL40x0_Device.ReadAmbiOnDemand (&AmbiValue);      // read ambi value on demand
        mled0 = 0;                                          // LED off

        // pront values on screen
        pc.printf("\nProxi: %5.0i cts \tAmbi: %5.0i cts \tIlluminance: %7.2f lx", ProxiValue, AmbiValue, AmbiValue/4.0);
    }
}
# endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// main #2
// Proximity Measurement in selftimed mode with 4 measurements/s
// Read prox value if ready with conversion, endless loop
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
# ifdef MAIN2

int main() {
    unsigned char ID=0;
    unsigned char Command=0;
    unsigned char Current=0;
    unsigned int  ProxiValue=0;
    unsigned int  AmbiValue=0;

    pc.baud(BAUD);

    // print information on screen
    pc.printf("\n\n VCNL4010/4020/3020 Proximity/Ambient Light Sensor");
    pc.printf("\n library tested with mbed LPC1768 (ARM Cortex-M3 core) on www.mbed.org");
    pc.printf(VERSION);
    pc.printf("\n Demonstration #2:");
    pc.printf("\n Proximity Measurement in selftimed mode with 4 measurements/s");
    pc.printf("\n Read prox value if ready with conversion, endless loop");

    VCNL40x0_Device.ReadID (&ID);                           // Read VCNL40x0 product ID revision register
    pc.printf("\n\n Product ID Revision Register: %d", ID);

    VCNL40x0_Device.SetCurrent (20);                        // Set current to 200mA
    VCNL40x0_Device.ReadCurrent (&Current);                 // Read back IR LED current
    pc.printf("\n IR LED Current: %d\n\n", Current);

    VCNL40x0_Device.SetProximityRate (PROX_MEASUREMENT_RATE_4); // set proximity rate to 4/s

    // enable prox in selftimed mode
    VCNL40x0_Device.SetCommandRegister (COMMAND_PROX_ENABLE | COMMAND_SELFTIMED_MODE_ENABLE);

    wait_ms(3000);                                          // wait 3s (only for display)

// endless loop /////////////////////////////////////////////////////////////////////////////////////

    while (1) {

        // wait on prox data ready bit
        do {
            VCNL40x0_Device.ReadCommandRegister (&Command); // read command register
        } while (!(Command & COMMAND_MASK_PROX_DATA_READY));// prox data ready ?

        mled0 = 1;                                          // LED on
        VCNL40x0_Device.ReadProxiValue (&ProxiValue);       // read prox value 
        mled0 = 0;                                          // LED off

        // print values on screen
        pc.printf("\nProxi: %5.0i cts", ProxiValue);
    }
}
# endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// main #3
// Proximity Measurement in selftimed mode with 31 measurements/s
// Interrupt waiting on proximity value > upper threshold limit
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
# ifdef MAIN3

int main() {
    unsigned char ID=0;
    unsigned char Command=0;
    unsigned char Current=0;
    unsigned int  ProxiValue=0;
    unsigned int  AmbiValue=0;
    unsigned char InterruptStatus=0;
    unsigned char InterruptControl=0;

    pc.baud(BAUD);

    // print information on screen
    pc.printf("\n\n VCNL4010/4020/3020 Proximity/Ambient Light Sensor");
    pc.printf("\n library tested with mbed LPC1768 (ARM Cortex-M3 core) on www.mbed.org");
    pc.printf(VERSION);
    pc.printf("\n Demonstration #3:");
    pc.printf("\n Proximity Measurement in selftimed mode with 31 measurements/s");
    pc.printf("\n Interrupt waiting on proximity value > upper threshold limit");

    VCNL40x0_Device.ReadID (&ID);                           // Read VCNL40x0 product ID revision register
    pc.printf("\n\n Product ID Revision Register: %d", ID);

    VCNL40x0_Device.SetCurrent (20);                        // Set current to 200mA
    VCNL40x0_Device.ReadCurrent (&Current);                 // Read back IR LED current
    pc.printf("\n IR LED Current: %d\n\n", Current);

    // stop all activities (necessary for changing proximity rate, see datasheet)
    VCNL40x0_Device.SetCommandRegister (COMMAND_ALL_DISABLE);

    // set proximity rate to 31/s
    VCNL40x0_Device.SetProximityRate (PROX_MEASUREMENT_RATE_31);

    // enable prox in selftimed mode
    VCNL40x0_Device.SetCommandRegister (COMMAND_PROX_ENABLE |
                                        COMMAND_SELFTIMED_MODE_ENABLE);

    // set interrupt control register
    VCNL40x0_Device.SetInterruptControl (INTERRUPT_THRES_SEL_PROX |
                                         INTERRUPT_THRES_ENABLE |
                                         INTERRUPT_COUNT_EXCEED_1);

    VCNL40x0_Device.ReadInterruptControl (&InterruptControl);                 // Read back Interrupt Control register
    pc.printf("\n Interrupt Control Register: %i\n\n", InterruptControl);


    // wait on prox data ready bit
    do {
        VCNL40x0_Device.ReadCommandRegister (&Command);     // read command register
    } while (!(Command & COMMAND_MASK_PROX_DATA_READY));    // prox data ready ?

    VCNL40x0_Device.ReadProxiValue (&ProxiValue);           // read prox value
    VCNL40x0_Device.SetHighThreshold (ProxiValue+100);      // set high threshold for interrupt

    wait_ms(3000);                                          // wait 3s (only for display)

// endless loop /////////////////////////////////////////////////////////////////////////////////////

    while (1) {

        // wait on prox data ready bit
        do {
            VCNL40x0_Device.ReadCommandRegister (&Command); // read command register
        } while (!(Command & COMMAND_MASK_PROX_DATA_READY));// prox data ready ?

        mled0 = 1;                                          // LED on
        VCNL40x0_Device.ReadProxiValue (&ProxiValue);       // read prox value
        mled0 = 0;                                          // LED off

        // read interrupt status register
        VCNL40x0_Device.ReadInterruptStatus (&InterruptStatus);

        // print prox value and interrupt status on screen
        pc.printf("\nProxi: %5.0i cts \tInterruptStatus: %i", ProxiValue, InterruptStatus);

        // check interrupt status for High Threshold
        if (InterruptStatus & INTERRUPT_MASK_STATUS_THRES_HI) {
            mled1 = 1;                                             // LED on
            VCNL40x0_Device.SetInterruptStatus (InterruptStatus);  // clear Interrupt Status
            mled1 = 0;                                             // LED on
        }
    }
}
# endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// main #4
// Proximity Measurement and Ambient light Measurement in selftimed mode
// Proximity with 31 measurements/s, Ambient light with 2 measurement/s
// Interrupt waiting on proximity value > upper threshold limit
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
# ifdef MAIN4

int main() {
    unsigned int i=0;
    unsigned char ID=0;
    unsigned char Command=0;
    unsigned char Current=0;
    unsigned int  ProxiValue=0;
    unsigned int  SummeProxiValue=0;
    unsigned int  AverageProxiValue=0;
    unsigned int  AmbiValue=0;
    unsigned char InterruptStatus=0;
    unsigned char InterruptControl=0;

    pc.baud(BAUD);

    // print information on screen
    pc.printf("\n\n VCNL4010/4020/3020 Proximity/Ambient Light Sensor");
    pc.printf("\n library tested with mbed LPC1768 (ARM Cortex-M3 core) on www.mbed.org");
    pc.printf(VERSION);
    pc.printf("\n Demonstration #4:");
    pc.printf("\n Proximity Measurement and Ambient light Measurement in selftimed mode");
    pc.printf("\n Proximity with 31 measurements/s, Ambient light with 2 measurement/s");
    pc.printf("\n Interrupt waiting on proximity value > upper threshold limit");

    VCNL40x0_Device.ReadID (&ID);                           // Read VCNL40x0 product ID revision register
    pc.printf("\n\n Product ID Revision Register: %d", ID);

    VCNL40x0_Device.SetCurrent (20);                        // Set current to 200mA
    VCNL40x0_Device.ReadCurrent (&Current);                 // Read back IR LED current
    pc.printf("\n IR LED Current: %d", Current);

    // stop all activities (necessary for changing proximity rate, see datasheet)
    VCNL40x0_Device.SetCommandRegister (COMMAND_ALL_DISABLE);

    // set proximity rate to 31/s
    VCNL40x0_Device.SetProximityRate (PROX_MEASUREMENT_RATE_31);

    // enable prox and ambi in selftimed mode
    VCNL40x0_Device.SetCommandRegister (COMMAND_PROX_ENABLE |
                                        COMMAND_AMBI_ENABLE |
                                        COMMAND_SELFTIMED_MODE_ENABLE);

    // set interrupt control for threshold
    VCNL40x0_Device.SetInterruptControl (INTERRUPT_THRES_SEL_PROX |
                                         INTERRUPT_THRES_ENABLE |
                                         INTERRUPT_COUNT_EXCEED_1);

    // set ambient light measurement parameter
    VCNL40x0_Device.SetAmbiConfiguration (AMBI_PARA_AVERAGE_32 |
                                          AMBI_PARA_AUTO_OFFSET_ENABLE |
                                          AMBI_PARA_MEAS_RATE_2);

    // measure average of prox value
    SummeProxiValue = 0;

    for (i=0; i<30; i++) {                                      
        do {                                                    // wait on prox data ready bit
            VCNL40x0_Device.ReadCommandRegister (&Command);     // read command register
        } while (!(Command & COMMAND_MASK_PROX_DATA_READY));    // prox data ready ?

        VCNL40x0_Device.ReadProxiValue (&ProxiValue);           // read prox value 
    
        SummeProxiValue += ProxiValue;                          // Summary of all measured prox values
    }

    AverageProxiValue = SummeProxiValue/30;                     // calculate average

    VCNL40x0_Device.SetHighThreshold (AverageProxiValue+100);   // set upper threshold for interrupt
    pc.printf("\n Upper Threshold Value: %i cts\n\n", AverageProxiValue+100);
 
    wait_ms(2000);                                              // wait 2s (only for display)

// endless loop /////////////////////////////////////////////////////////////////////////////////////

    while (1) {

        // wait on data ready bit
        do {
            VCNL40x0_Device.ReadCommandRegister (&Command); // read command register
        } while (!(Command & (COMMAND_MASK_PROX_DATA_READY | COMMAND_MASK_AMBI_DATA_READY))); // data ready ?

        // read interrupt status register
        VCNL40x0_Device.ReadInterruptStatus (&InterruptStatus);

        // check interrupt status for High Threshold
        if (InterruptStatus & INTERRUPT_MASK_STATUS_THRES_HI) {
            mled2 = 1;                                             // LED on, Interrupt
            VCNL40x0_Device.SetInterruptStatus (InterruptStatus);  // clear Interrupt Status
            mled2 = 0;                                             // LED off, Interrupt
        }

        // prox value ready for using
        if (Command & COMMAND_MASK_PROX_DATA_READY) {
            mled0 = 1;                                      // LED on, Prox Data Ready
            VCNL40x0_Device.ReadProxiValue (&ProxiValue);   // read prox value

            // print prox value and interrupt status on screen
            pc.printf("\nProxi: %5.0i cts \tInterruptStatus: %i", ProxiValue, InterruptStatus);

            mled0 = 0;                                      // LED off, Prox data Ready
        }

        // ambi value ready for using
        if (Command & COMMAND_MASK_AMBI_DATA_READY) {
            mled1 = 1;                                      // LED on, Ambi Data Ready
            VCNL40x0_Device.ReadAmbiValue (&AmbiValue);     // read ambi value

            // print ambi value and interrupt status on screen
            pc.printf("\n                                               Ambi: %i", AmbiValue);

            mled1 = 0;                                      // LED off, Ambi Data Ready
        }
    }
}
# endif

# ifdef MAIN5

int main() {
    unsigned char ID=0;
    unsigned char Current=0;
    unsigned int  ProxiValue=0;

    pc.baud(BAUD);                                          // set USB speed (virtual COM port)

    // print information on screen
    pc.printf("\n\n VCNL4010/4020/3020 Proximity/Ambient Light Sensor");
    pc.printf("\n library tested with mbed LPC1768 (ARM Cortex-M3 core) on www.mbed.org");
    pc.printf(VERSION);
    pc.printf("\n Demonstration #5:");
    pc.printf("\n Read Proximity on demand in an endless loop");

    VCNL40x0_Device.ReadID (&ID);                           // Read VCNL40x0 product ID revision register
    pc.printf("\n\n Product ID Revision Register: %d", ID);

    VCNL40x0_Device.SetCurrent (20);                        // Set current to 200mA
    VCNL40x0_Device.ReadCurrent (&Current);                 // Read back IR LED current
    pc.printf("\n IR LED Current: %d\n\n", Current);

    wait_ms(3000);                                          // wait 3s (only for display)

// endless loop /////////////////////////////////////////////////////////////////////////////////////

    while (1) {
        mled0 = 1;                                          // LED on
        VCNL40x0_Device.ReadProxiOnDemand (&ProxiValue);    // read prox value on demand
        mled0 = 0;                                          // LED off

        // pront values on screen
        pc.printf("\nProxi: %5.0i cts, ProxiValue",ProxiValue);
    }
}
# endif
