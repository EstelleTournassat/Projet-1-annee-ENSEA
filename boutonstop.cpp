/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty (2bndy5)
 */

/**
 * A simple example of sending data from 1 nRF24L01 transceiver to another.
 *
 * This example was written to be used on 2 devices acting as "nodes".
 * Use `ctrl+c` to quit at any time.
 */
#include <ctime>       // time()
#include <iostream>    // cin, cout, endl
#include <string>      // string, getline()
#include <time.h>      // CLOCK_MONOTONIC_RAW, timespec, clock_gettime()
#include <RF24/RF24.h> // RF24, RF24_PA_LOW, delay()

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
float payload = 0.0;

void setRole(); // prototype to set the node's role
void master();  // prototype of the TX node's behavior
void slave();   // prototype of the RX node's behavior

// custom defined timer for evaluating transmission time in microseconds
struct timespec startTimer, endTimer;
uint32_t getMicros(); // prototype to get elapsed time in microseconds

int main(int argc, char** argv)
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

    // Set the radioNumber and the role of the raspberry via the terminal on startup
    if (argc==1){}
    else if (argc==2){
        role=argv[1];}
    else if (argc==3){
        role=argv[1];
        canal=argv[2];}
    
    if (canal == std::string("0")){
        Canal=0;
    }
    else{Canal=1;}
    string Canal;
    radioNumber = Canal.length() > 0 && (uint8_t)Canal[0] == 49;

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
    setRole(); // calls master() or slave() based on user input
    return 0;
}

/**
 * set this node's role from stdin stream.
 * this only considers the first char as input.
 */
void setRole()
{
    if (role == std::string("R")){
        slave();
    }
    else{master();}
} // setRole()

/**
 * make this node act as the transmitter
 */
void master()
{
    radio.stopListening(); // put radio in TX mode

    string input = "";
    cout << "*** Press 'S' to stop the robot. Press any other key to do nothing.\n";
    if (input[0] == 'S' || input[0] == 's'){
        payload=1.0;}
    else {payload=0.0;}

    unsigned int failure = 0; // keep track of failures
    while (failure < 6) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &startTimer);    // start the timer
        bool report = radio.write(&payload, sizeof(float)); // transmit & save the report
        uint32_t timerElapsed = getMicros();                // end the timer

        if (report) {
            // payload was delivered
            cout << "Transmission successful! Time to transmit = ";
            cout << timerElapsed;                     // print the timer result
            if (payload>0.5){
                cout << " The robot stopp "<< endl; // print payload sent
            }
            else {
                cout << " Nothing happened "<< endl; // print payload sent
            }
        }
        else {
            // payload was not delivered
            cout << "Transmission failed or timed out" << endl;
            failure++;
        }

        // to make this example readable in the terminal
        delay(1000); // slow transmissions down by 1 second
    }
    cout << failure << " failures detected. Leaving TX role." << endl;
}

/**
 * make this node act as the receiver
 */
void slave()
{

    radio.startListening(); // put radio in RX mode

    while (1) { 
        uint8_t pipe;
        if (radio.available(&pipe)) {                        // is there a payload? get the pipe number that recieved it
            uint8_t bytes = radio.getPayloadSize();          // get the size of the payload
            radio.read(&payload, bytes);                     // fetch payload from FIFO
            cout << "Received " << (unsigned int)bytes;      // print the size of the payload
            cout << " bytes on pipe " << (unsigned int)pipe; // print the pipe number
            cout << ": " << payload << endl;                 // print the payload's value
        }
    cout << "Nothing received." << endl
    delay(1000);
    }
    radio.stopListening();
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
