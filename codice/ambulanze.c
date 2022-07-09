#include <stdio.h>
#include <math.h>
#include "rngs.h"
#include "nodi.h"


int indexWrite1 = 0;
int indexWrite2 = 0;
int indexRead1 = 0;
int indexRead2 = 0;

int NextEvent_Ambulance(event_list_Priority event[]){

    int e = - 1;
    int i = 1;

    //la dimensione della lista è SERVERS_Ambulance+1
    while (event[i].x == 0 && i < SERVERS_Ambulance+1)          /* find the index of the first 'active' */
        i++;                                                    /* element in the event list            */
    if (i == SERVERS_Ambulance+1) return e;
    e = i;

    while (i < SERVERS_Ambulance) {                             /* now, check the others to find which  */
        i++;                                                        /* event type is most imminent          */
        if ((event[i].x == 1) && (event[i].t < event[e].t)){
            e = i;
        }
    }

    return (e);
}

int GenerateType (){

    // 1 urgente; 0 non urgente
    SelectStream(2);
    if (Random() <= 0.38)
        return (1);
    else
        return (0);
}

int GenerateType_From_Ambulance (){

    // 2 rosso; 1 giallo; 0 verde
    SelectStream(6);
    if (Random() < 0.13)
        return (2);
    else if (Random() < 0.78)
        return (1);                     //0.65 + 0.13
    else
        return (0);
}


double GetService_Ambulance(){

    SelectStream(3);
    return(Exponential(30.0));
}


int generateDeath(){

    // 1 decesso; 0 altrimenti
    SelectStream(4);
    if (Random() < 0.02)
        return (1);
    else
        return (0);
}


void arrivalAtAmbulance(double current, event_list_Priority event_Ambulance[], sum_server_Priority sum_Ambulance[], statoAmbulance* statoAmbulanza ){ //processamento arrivo ambulanza

    //printf("Processing ambulance arrival, next at: %f\n", event_2[0].t);
    //genero la size del job in modo da sapere qual'è la sua priorità
    statoAmbulanza -> numberNode++;
    int type;
    double service  = GetService_Ambulance();
    if(service<=30.0)
        type=0;
    else{
        type=1;
        statoAmbulanza-> number_critical_Node++;
    }
    event_Ambulance[0].type = type;                               //Memorizzo il tipo del job arrivato

    // se ho abbastanza serventi l'arrivo viene servito immediatamente
    if (statoAmbulanza-> numberNode <= SERVERS_Ambulance) {
        int  s = FindOneAmbulance(event_Ambulance);                 // Trovo il servente libero da piu' tempo
        sum_Ambulance[s].service += service;                        // Aggiungo il tempo di servizio alla somma dei precedenti tempi di servizio per il server s
        sum_Ambulance[s].served++;                                  // Aumento il numero di jobs serviti al server s
        if (type) sum_Ambulance[s].servedCritical++;                // Se l'evento è critico aumento il numero di jobs "critici" per il server s


        event_Ambulance[s].t = current + service;                   // Genero il prossimo completamento del servente s
        event_Ambulance[s].x = 1;                                   // Imposto il servente s busy
        event_Ambulance[s].type = type;                             // Memorizzo il tipo di arrivo

        //printf("    entro in servizio nel servente (%d) libero, servizio a : %f\n", s, event_Ambulance[s].t);

    } else {
        // L'arrivo è aggiunto alla coda
        statoAmbulanza->numberQueue++;
        if (type){
            statoAmbulanza->number_critical_Queue++;
            queueCritical[indexWrite1] = service;
            indexWrite1++;
        }else{
            queueNotCritical[indexWrite2] = service;
            indexWrite2++;
        }

    }
}


void completionAtAmbulance (double current, event_list_Priority event_Ambulance[],
                             sum_server_Priority sum_Ambulance[], statoAmbulance* statoAmbulanza, stato118* stato118, int e){

    statoAmbulanza->numberCompletion ++;          // Aumento il numero di completamenti del sistema ambulance
    statoAmbulanza->numberNode --;                //Diminuisco il numero di jobs nel sistema ambulanze
    //printf("Processing ambulance completion: %d job rimanenti (%d nel 118)\n", statoAmbulanza->numberNode, stato118->numberNode);

    if(event_Ambulance[e].type) statoAmbulanza->number_critical_Node --;
    int s = e;
    int type = 0;

    // Se ho qualcuno in coda prendo il prossimo job e ne genero il servizio (in base al tipo)
    // Quelli critici vengono schedulati prima con il rischio di avere starvation
    // NB: poichè avevo coda solo il server s è disponibile

    if (statoAmbulanza->numberNode >= SERVERS_Ambulance) {

        double service;
        statoAmbulanza -> numberQueue --;
        // Se nella coda c'è almeno un paziente critico ...
        if (statoAmbulanza -> number_critical_Queue > 0) {
            type = 1;
            statoAmbulanza-> number_critical_Queue --;
            service = queueCritical[indexRead1];
            indexRead1++;
            //printf("    Entra un paziente critico\n");
        } else{
            service = queueNotCritical[indexRead2];
            indexRead2++;
            //printf("    Entra un paziente normale\n");
        }

        // non va bene: //double service = GetService_Ambulance(type);

        sum_Ambulance[s].service += service;                        // Aumento l'accumulated service times per il server s
        sum_Ambulance[s].served++;                                  // Aumento il numero di jobs serviti dal server s

        if (type) sum_Ambulance[s].servedCritical++;                // Sto servendo un job critico, aumento il contatore

        event_Ambulance[s].t = current + service;                   // Aggiorno il prossimo completamento per il server s
        event_Ambulance[s].type = type;
        //printf("    servizio = %lf\n", event_Ambulance[s].t);
        //printf("[SMALTISCO IL JOB NEL SERVENTE %d]\n",s);

    } else
        event_Ambulance[s].x = 0;                                   // Non ho coda, riporto il server a idle
}

void SetupNode_Ambulance (event_list_Priority* event,sum_server_Priority* sum){

    for (int s = 0; s <= SERVERS_Ambulance; s++) {
        event[s].t    = 0.0;          /* this value is arbitrary because */
        if(s==0)
            event[s].x = 1;                // tanto non lo utilizziamo. Lo sfrutto per dire che gli arrivi all'abulanza sono abilitati
        else
            event[s].x      = 0;              /* all servers are initially idle  */
        event[s].type  = 0;
        sum[s].service = 0.0;
        sum[s].servedCritical = 0.0;
        sum[s].served = 0.0;
    }
}

int FindOneAmbulance(event_list_Priority event[])
/* -----------------------------------------------------
 * return the index of the available server idle longest
 * end index excluded
 * -----------------------------------------------------
 */
{
    int s;
    int i = 1;

    while (event[i].x == 1)       /* find the index of the first available */
        i++;                        /* (idle) server                         */
    s = i;

    while (i < SERVERS_Ambulance) {         /* now, check the others to find which   */
        i++;                        /* has been idle longest                 */
        if ((event[i].x == 0) && (event[i].t < event[s].t))
            s = i;
    }
  return (s);
}
