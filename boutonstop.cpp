/*
 * Code fait a partir de gettingstarted de la librairie nrf24
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 */

/**
 * Transmission entre une carte receveur qui lit en continue et une Raspberry 
 * Transmetteur qui envoie une commande ou rien ne se passe et sinon la commande stop
 * Utilise ctrl +c pour quitter le programme
 */
#include <ctime>       // time()
#include <iostream>    // cin, cout, endl
#include <string>      // string, getline()
#include <time.h>      // CLOCK_MONOTONIC_RAW, timespec, clock_gettime()
#include <RF24/RF24.h> // RF24, RF24_PA_LOW, delay()

using namespace std;

/****************** Linux ***********************/
// Radio CE Pin, CSN Pin, SPI Speed
// CE Pin utilise le nombre GPIO avec BCM et les SPIDEV drivers
// Les adresses CS Pin donne le nombre du bus SPI à /dev/spidev<a>.<b>
// ie: RF24 radio(<ce_pin>, <a>*10+<b>); spidev1.0 is 10, spidev1.1 is 11 etc..
#define CSN_PIN 0
#ifdef MRAA
    #define CE_PIN 15 // GPIO22
#else
    #define CE_PIN 22
#endif
// Generic:
RF24 radio(CE_PIN, CSN_PIN);

int payload = 0.0;
char* role;
char* canal;
int Canal=0;
void master();  // prototype of the transmetteur
void slave();   // prototype of the receveur
void SetRole(); //prototype pour attribuer les rôles

// custom defined timer for evaluating transmission time in microseconds
struct timespec startTimer, endTimer;

int main(int argc, char* const argv[])
{

    // check l'installation de la radio
    if (!radio.begin()) {
        cout << "La nfr24 ne répond pas!!" << endl;
        return 0; // quit now
    }
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

    // pour utiliser différentes adresses sur une paire de radios, nous avons besoin d'une variable
    // pour identifier de manière unique l'adresse que cette radio utilisera pour transmettre
    bool radioNumber = 1; // 0 utilise l'adresse [0] pour transmettre, 1 utilise l'adresse [1] pour transmettre

    // Laisser ces adresses être utilisées pour la paire
    uint8_t address[2][6] = {"1Node", "2Node"};
    // Il est très utile de considérer une adresse comme un chemin plutôt que 
    //comme une destination de périphérique d'identification

    // Définissez le radioNumber via le terminal au démarrage
    string Canal;
    radioNumber = Canal.length() > 0 && (uint8_t)Canal[0] == 49;

    // économisez sur le temps de transmission en configurant la radio pour ne transmettre que 
    // le nombre d'octets dont nous avons besoin pour transmettre un flottant
    radio.setPayloadSize(sizeof(payload));

    // Définissez le niveau PA bas pour essayer d'éviter les problèmes liés à l'alimentation, 
    // car ces exemples sont probablement exécutés avec des nœuds à proximité les uns des autres
    radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

    // définir l'adresse TX du nœud RX dans le canal TX
    radio.openWritingPipe(address[radioNumber]); // utilise toujours le canal 0

    // définir l'adresse RX du nœud TX dans un canal RX
    radio.openReadingPipe(1, address[!radioNumber]); // utile le canal 1
    SetRole();
    return 0;
}

void SetRole(){
    if (role == std::string("R")){
        slave();
    }
    else{
        master();
    }
}
/**
 * fonction du transmetteur
 */
void master()
{
    radio.stopListening(); // met la radio en mode TX
    string input = "";
    cout << "*** Presser la touche 'S' pour stopper le robot\n";
    cout << "*** Presser une autre touche pour ne rien faire\n";
    getline(cin, input);

    if (input[0] == 'S' || input[0] == 's'){
        payload=1.0;}
    else {payload=0.0;}
    cout <<payload<<endl;

    unsigned int failure = 0; // on regarde le nombre d'échec
    while (failure < 20) {
        bool report = radio.write(&payload, sizeof(float)); // transmettre & sauver le message

        if (report) {
            if (payload==1){ //message transmit
                cout << "Commande stop activée"<< endl;
            }
            else {cout << "On ne fait rien"<< endl; }
            
        }
        else {
            // messaqge non transmis
            cout << "Pas de transmission ou temps trop long " << endl;
            failure++;
        }

        delay(1000); // ralentit la transmission de 1 seconds
    }
    cout << failure << " Erreurs détectées. On arrête la transmission." << endl;
} 

/**
 * fonction du receveur
 */
void slave()
{
    while(1){
        time_t startTimer = time(nullptr);       // start a timer
        while (time(nullptr) - startTimer < 6) { // use 6 second timeout
            uint8_t pipe;
            if (radio.available(&pipe)) {                        // is there a payload? get the pipe number that recieved it
                cout << payload << endl;                 		// print the payload's value
                startTimer = time(nullptr);                      // reset timer
            }
        }
        cout << "Nothing received in 6 seconds." << endl;
    }
}
