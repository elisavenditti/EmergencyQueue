# EmergencyQueuePMCSN: dettagli sull'esecuzione
Il makefile genera l'eseguibile (a.out in linux). Per cambiare le configurazioni del sistema (come numero di serventi, tipologia di simulazione, etc.) bisogna modificare le costanti definite nel file nodi.h. Questo consente di avere una panoramica completa in un unico file senza dover specificare le caratteristiche di simulazione nelle direttive di compilazione.

#define SERVERS_118  		-->	intero positivo

#define SERVERS_Ambulance 	-->	intero positivo

#define SERVERS_ProntoSoccorso 	-->	intero positivo

#define SERVERS_ProntoSoccorso_2-->	intero positivo

#define SERVERS_Accettazione 	-->	intero positivo

#define SERVERS_Accettazione_2 	-->	intero positivo

#define IMPROVEMENT_MODEL  	-->	0 (false), 1(true)

#define FINITE_HORIZON  	-->	0 (false), 1(true)

#define N_CENTRI 		-->	6 

nella simulazone a orizzonte finito decommentare:
#define STOP        720.0

nella simulazone a orizzonte infinito decommentare:
#define STOP        200000.0
