#include <stdio.h>
#include <math.h>
#include "rngs.h"
#include "nodi.h"


int generatePassing_2(){
    //1 morte; 0 altrimenti
    SelectStream(15);
    if (Random() < 0.02)
        return (1);
    else
        return (0);
}

void arrivalAtProntoSoccorso_2(double current, int type, event_list_Priority event[], sum_server_Priority sum[], statoProntoSoccorso* statoProntoSoccorso, long* deathPS ){

    if(generatePassing_2()){        // non incremento nessuna variabile del pronto soccorso perchè ho un'uscita dal sistema
        (*deathPS) ++;
        return;
    }

    statoProntoSoccorso -> numberNode++;
    //printf("JOB IN PRONTO SOCCORSO: %d\n",statoProntoSoccorso -> numberNode);

    if (type == 2){
        statoProntoSoccorso -> number_Red_Node ++;
        //printf("Processing Pronto Soccorso arrival: RED\n");
    } else if (type == 1) {
        statoProntoSoccorso -> number_Yellow_Node++;
        //printf("Processing Pronto Soccorso arrival: YELLOW\n");
    }

    event[0].type = type;                                     //Memorizzo il tipo del job arrivato

    // se ho a disposizione almeno un server l'arrivo è servito immediatamente (in base al tipo)
    if (statoProntoSoccorso -> numberNode <= SERVERS_ProntoSoccorso_2) {

        double service  = GetService_ProntoSoccorso_2(type);
        int  s = FindOneProntoSoccorso_2(event);                  // Trovo il server disponibile da piu' tempo
        sum[s].service += service;                              // Aggiungo il tempo di servizio alla somma dei precedenti tempi di servizio per il server s
        sum[s].served++;                                        // Aumento il numero di jobs serviti al server s
        if (type) sum[s].servedCritical++;                      // Se l'evento è critico aumento il numero di jobs
                                                                // critici (gialli e rossi) per il server s
        event[s].t = current + service;                         // Genero il prossimo completamento al server s
        event[s].x = 1;                                         // Server s busy
        event[s].type = type;                                   // Memorizzo il tipo di completamento del prossimo servizio
        //printf("    entro in servizio nel servente (%d) libero, servizio a : %f\n", s, event[s].t);
    } else {
        // L'arrivo è aggiunto alla coda
        statoProntoSoccorso->numberQueue++;
        if (type == 2) statoProntoSoccorso -> number_Red_Queue++;
        else if (type == 1) statoProntoSoccorso -> number_Yellow_Queue++;
        //printf("    vado in coda\n");
    }
}


void completionAtProntoSoccorso_2 (double current, event_list_Priority event[], sum_server_Priority sum[], statoProntoSoccorso* statoProntoSoccorso, statoAmbulance* statoAmbulanza, double* stopPS_2, int e){

    statoProntoSoccorso->numberNode --;                               // Diminuisco il numero di jobs nel sistema pronto soccorso
    statoProntoSoccorso->numberCompletion ++;                         // Aumento il numero di completamenti del sistema pronto soccorso
    //printf("Processing Pronto soccorso service: %d job rimanenti (%d nelle Ambulanze)\n", statoProntoSoccorso->numberNode, statoAmbulanza->numberNode);

    if(event[e].type == 2) {
        statoProntoSoccorso ->number_Red_Node --;
        statoProntoSoccorso ->number_Red_Completion ++;
    } else if (event[e].type == 1) {
        statoProntoSoccorso->number_Yellow_Node --;
        statoProntoSoccorso ->number_Yellow_Completion ++;
    }

    int s = e;
    int type = 0;                                                     // tipo di job che prenderò in servizio

    // se ho coda prendo un job e lo servo (i job critici sono schedulati prima con il rischio di avere starvation
    // NB: poichè c'è coda solo il server che si è appena liberato è disponibile
    if (statoProntoSoccorso->numberNode >= SERVERS_ProntoSoccorso_2) {
        statoProntoSoccorso ->numberQueue --;

        // servo i job con più priorità: codici rossi
        if (statoProntoSoccorso ->number_Red_Queue > 0) {
            type = 2;
            statoProntoSoccorso->number_Red_Queue  --;
            //printf("    Entra un paziente con codice rosso\n");
        } else if(statoProntoSoccorso ->number_Yellow_Queue > 0){
            type = 1;
            statoProntoSoccorso->number_Yellow_Queue  --;
            //printf( "    Entra un paziente con codice giallo\n");
        }
        double service   = GetService_ProntoSoccorso_2();

        sum[s].service += service;                                      // Aumento l'accumulated service times per il server s
        sum[s].served++;                                                // Aumento il numero di jobs serviti dal server s
        if (type) sum[s].servedCritical++;                              // Sto servendo un job critico (gialli e rossi): aumento il contatore.

        event[s].t = current + service;                                 // Aggiorno il prossimo completamento per il server s
        event[s].type = type;
        // printf("    servizio = %lf\n", event[s].t);
        // printf("[SMALTISCO IL JOB NEL SERVENTE %d]\n",s);
    } else
        event[s].x = 0;

    if (event[0].x == 0 && event[1].x == 0 && statoProntoSoccorso->numberNode == 0){
        *stopPS_2 = current;
    }
}

void SetupNode_ProntoSoccorso_2 (event_list_Priority* event,sum_server_Priority* sum){

    for (int s = 0; s <  SERVERS_ProntoSoccorso_2 + 2; s++) {
        event[s].t    = 0.0;
        if(s == 0 || s == 1) event[s].x = 1;        // indicano che gli arrivi al pronto soccorso sono abilitati
        else event[s].x      = 0;
        event[s].type  = 0;
        sum[s].service = 0.0;
        sum[s].servedCritical = 0.0;
        sum[s].served = 0.0;
    }
}

int FindOneProntoSoccorso_2(event_list_Priority event[]){
    int s;
    int i = 2; // Parto da due, i primi due indici rappresentano gli arrivi

    while (event[i].x == 1)       /* find the index of the first available */
        i++;                        /* (idle) server                         */
    s = i;

    while (i < SERVERS_ProntoSoccorso_2 + 1) {         /* now, check the others to find which   */
        i++;                        /* has been idle longest                 */
        if ((event[i].x == 0) && (event[i].t < event[s].t))
        s = i;
    }
    return (s);
}


double GetService_ProntoSoccorso_2(){
    SelectStream(14);
    return Exponential(25.0);
}



