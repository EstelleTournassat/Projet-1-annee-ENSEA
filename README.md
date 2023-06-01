## Projet-1-année-ENSEA

Projet de télémetrie en lien avec le projet CoHoMa.

# Description :
Les communications entre un robot et son téléopérateur utilisent un réseau wifi maillé (mesh 802.11s). Bien que très robuste, la portée de ce réseau n’est pas infinie (maximum 50m par balise), et une perte de connexion peut survenir même à plus faible distance, notamment à cause du relief ou de la végétation.
L’objectif est donc de mettre en place un réseau secondaire, de plus forte portée mais de débit réduit, permettant d’assurer:

-> la continuité de la téléopération du robot en cas de perte de la liaison wifi  
-> l’arrêt d’urgence des moteurs a la demande ou en cas de perte totale de connexion  
-> la remontée de la position GPS du robot (télémetrie)  
-> Le tout pourra être opéré sur batterie.  


Ce réseau secondaire va utiliser les composants NRF24L01 de Nordic Semiconductor, et devra faire communiquer des STM32 et des cartes de type Raspberry pi zero.


# Objectifs :
-> mise en œuvre du réseau nrf24  
-> réalisation du module autonome: batterie, boitier, PCB  
-> conception de l’arrêt d’urgence et du télépilotage et intégration du GPS  


# Les délais :
-> 17/04 : GANTT (format excel) + spécifications techniques/diagramme d’architecture (format pdf)  
-> 09/05 : Auto-évaluation des compétences  
-> 06/06 : Livrable final et Soutenance finale  

# Les tâches :
-> Documentation sur le système de réseau nrf24  


# Répartition des tâches :
Cf GANTT 
