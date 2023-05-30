/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Estelle TOURNASSAT
 */

/**
 * Code pour un nrF24 en tant que transmetteur: si on appuie sur la touche s du clavier, il envoie: 1
 * et si on appuie sur la touche p, il envoie: 0
 * Utiliser `ctrl+c` pour quitter .
 */
#include <ctime>       // time()
#include <iostream>    // cin, cout, endl
#include <string>      // string, getline()
#include <time.h>      // CLOCK_MONOTONIC_RAW, timespec, clock_gettime()
#include <RF24/RF24.h> // RF24, RF24_PA_LOW, delay()
#include <wiringPi.h>  //bibliothèque wiringPi

using namespace std;

/****************** Linux ***********************/
// Radio CE Pin, CSN Pin, SPI Speed
// CE Pin uses GPIO number with BCM and SPIDEV drivers, other platforms use their own pin numbering
// CS Pin addresses the SPI bus number at /dev/spidev<a>.<b>
// ie: RF24 radio(<ce_pin>, <a>*10+<b>); spidev1.0 is 10, spidev1.1 is 11 etc..
#define CSN_PIN 0
#ifdef MRAA
    #define CE_PIN 15 // GPIO22
#else
    #define CE_PIN 22
#endif
// Generic:
RF24 radio(CE_PIN, CSN_PIN);
/****************** Linux (BBB,x86,etc) ***********************/
// See http://nRF24.github.io/RF24/pages.html for more information on usage
// See http://iotdk.intel.com/docs/master/mraa/ for more information on MRAA
// See https://www.kernel.org/doc/Documentation/spi/spidev for more information on SPIDEV

// For this example, we'll be using a payload containing
// a single float number that will be incremented
// on every successful transmission
int payload = 0;

void setRole(); // prototype to set the node's role
void master();  // prototype of the TX node's behavior
void slave();   // prototype of the RX node's behavior

// custom defined timer for evaluating transmission time in microseconds
struct timespec startTimer, endTimer;
uint32_t getMicros(); // prototype to get elapsed time in microseconds

int main()
{
    // perform hardware check
    if (!radio.begin()) {
        cout << "radio hardware is not responding!!" << endl;
        return 0; // quit now
    }

    // to use different addresses on a pair of radios, we need a variable to
    // uniquely identify which address this radio will use to transmit
    bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit

    // print example's name
    cout << argv[0] << endl;

    // Let these addresses be used for the pair
    uint8_t address[2][6] = {"1Node", "2Node"};
    // It is very helpful to think of an address as a path instead of as
    // an identifying device destination

    // save on transmission time by setting the radio to only transmit the
    // number of bytes we need to transmit a float
    radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to
    // each other.
    radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

    // set the TX address of the RX node into the TX pipe
    radio.openWritingPipe(address[radioNumber]); // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1

    // For debugging info
    // radio.printDetails();       // (smaller) function that prints raw register values
    // radio.printPrettyDetails(); // (larger) function that prints human readable data

    // ready to execute program now
    master(); // calls master()
    return 0;
}



/**
 * make this node act as the transmitter
 */
void master()
{
    char r;
    radio.stopListening(); // put radio in TX mode
    unsigned int failure = 0; // keep track of failures
    while (failure < 6) {
        if((r=getchar()) == 'p'){

            payload=1;
            clock_gettime(CLOCK_MONOTONIC_RAW, &startTimer);    // start the timer
            bool report = radio.write(&payload, sizeof(float)); // transmit & save the report
            uint32_t timerElapsed = getMicros();               // end the timer

            if (report) {
            // payload was delivered
            cout << "Transmission successful! Time to transmit = ";
            cout << timerElapsed;                     // print the timer result
            cout << " us. Sent: " << payload << endl; // print payload sent
            payload += 0.01;                          // increment float payload
            }
            else {
            // payload was not delivered
            cout << "Transmission failed or timed out" << endl;
            failure++;
            }
        // to make this example readable in the terminal
        delay(1000); // slow transmissions down by 1 second
        }
        if((r=getchar()) == 's'){

            payload=0;
            clock_gettime(CLOCK_MONOTONIC_RAW, &startTimer);    // start the timer
            bool report = radio.write(&payload, sizeof(float)); // transmit & save the report
            uint32_t timerElapsed = getMicros();               // end the timer

            if (report) {
            // payload was delivered
            cout << "Transmission successful! Time to transmit = ";
            cout << timerElapsed;                     // print the timer result
            cout << " us. Sent: " << payload << endl; // print payload sent
            payload += 0.01;                          // increment float payload
            }
            else {
            // payload was not delivered
            cout << "Transmission failed or timed out" << endl;
            failure++;
            }
        // to make this example readable in the terminal
        delay(1000); // slow transmissions down by 1 second
        }
    }
    cout << failure << " failures detected. Leaving TX role." << endl;
}


/**
 * Calculate the elapsed time in microseconds
 */
uint32_t getMicros()
{
    // this function assumes that the timer was started using
    // `clock_gettime(CLOCK_MONOTONIC_RAW, &startTimer);`

    clock_gettime(CLOCK_MONOTONIC_RAW, &endTimer);
    uint32_t seconds = endTimer.tv_sec - startTimer.tv_sec;
    uint32_t useconds = (endTimer.tv_nsec - startTimer.tv_nsec) / 1000;

    return ((seconds)*1000 + useconds) + 0.5;
}