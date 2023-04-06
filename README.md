## Projet-1-ann-e-ENSEA

Projet de télémetrie en lien avec le projet CoHoMa.

# Description:
Les communications entre un robot et son t ́el ́eop ́erateur utilise un réseau wifi maillé (mesh 802.11s). Bien que tres robuste, la port ́ee de ce r ́eseau n’est pas infini (maximum 50m par balise), et une perte de connexion peut survenir mˆeme `a plus faible distance, notamment a cause du relief ou de la v ́eg ́etation.
L’objectif est donc de mettre en place un r ́eseau secondaire, de plus forte port ́ee mais de d ́ebit réduit, permettant d’assurer:

-> la continuit ́e de la teleoperation du robot en cas de perte de la liaison wifi.
-> l’arrêt d’urgence des moteurs `a la demande ou en cas de perte totale de connexion
-> la remontée de la position GPS du robot (télémetrie)
-> Le tout pourra être opéré sur batterie.


Ce réseau secondaire va utiliser les composants NRF24L01 de Nordic Semiconductor, et devra
faire communiquer des STM32 et des cartes de type Raspberry pi zero.


# Objectifs:
-> mise en œuvre du réseau nrf24
-> réalisation du module autonome: batterie, boitier, PCB
-> conception de l’arrêt d’urgence et du télépilotage et intégration du GPS


# Les délais (à respecter les gense svp!!!):
-> 17/04 : GANTT (format excel) + sp ́ecifications techniques/diagramme d’architecture (format pdf)
-> 09/05 : Auto-evaluation des competences
-> 06/06 : Livrable final et Soutenance finale

# Les tâches:
-> Documentation sur 


# Répartition des tâches:

Groupe PCB:
Groupe
