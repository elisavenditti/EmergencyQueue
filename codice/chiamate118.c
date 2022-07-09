#include <stdio.h>
#include <math.h>
#include "rngs.h"
#include "nodi.h"

double arrival = 0.0;

double GetArrival_118(){

    if (restart[0]) {
        arrival = 0.0;
        restart[0] = 0;
    }
    SelectStream(0);
    arrival += Exponential(15.0);
    return (arrival);
}

double GetArrival_118_BURST(){

    if (restart[0]) {
        arrival = 0.0;
        restart[0] = 0;
    }
    SelectStream(0);
    arrival += Exponential(5.0);
    return (arrival);
}


void completionAt118 (double current, event_list_No_Priority event_118[], event_list_Priority event_Ambulance[], stato118* stato118, statoAmbulance* statoAmbulanza,
                      sum_server_No_Priority sum_118[],sum_server_Priority sum_Ambulance[], double* stop118,
                      int e){

    stato118->numberNode--;                               // Diminuisco il numero di jobs nel sistema
    stato118->numberCompletion++;                         // Aumento il numero di completamenti per il 118

    //printf("Processing completion at 118, simultaneous ambulance arrival at: %f\n", event_118[e].t);

    int s = e;
    //printf("[SMALTISCO IL JOB NEL SERVENTE %d]\n",s);
    if (!IMPROVEMENT_MODEL)  event_118[s].x = 0;                                   //Il server è tornato idle

    //Aggiorno l'arrivo al sistema ambulanze come fosse istantaneo al completamento del job al 118
    event_Ambulance[0].t = event_118[s].t;
    //event_Ambulance[0].x = 1; //Arrivo valido


    if(IMPROVEMENT_MODEL && stato118->numberNode >= SERVERS_118){
        stato118->numberQueue--;
        double service = GetService_118();

        sum_118[s].service += service;                             // Aumento l'accumulated service times per il server s
        sum_118[s].served++;                                       // Aumento il numero di jobs serviti dal server s

        event_118[s].t = current + service;         // Aggiorno il prossimo completamento per il server s

    }else if(IMPROVEMENT_MODEL && stato118->numberNode < SERVERS_118)
        event_118[s].x = 0;                                   //Il server è tornato idle


    // Se nel 118 non ci sono piu' jobs da servire e non posso avere piu' arrivi al 118
    // significa che non posso piu' avere arrivi al nodo successivo
    if (event_118[0].x == 0 && stato118->numberNode == 0){
        *stop118 = current;
        event_Ambulance[0].x = 0;
    }
    arrivalAtAmbulance(current, event_Ambulance, sum_Ambulance, statoAmbulanza);
}


void arrivalAt118 (double current, event_list_No_Priority event_1[],sum_server_No_Priority sum_1[], stato118* stato118, long* reject){

    stato118 -> numberNode ++;                // aumento il numero di jobs nel sistema
    if(FINITE_HORIZON && current < BURST_TIME)
        event_1[0].t = GetArrival_118_BURST();
    else
        event_1[0].t = GetArrival_118();          // genero il prossimo arrivo al sistema 118

    //printf("Processing arrival at 118, next at: %f\n", event_1[0].t);

    if (event_1[0].t > STOP) event_1[0].x = 0; // Arrivi successivi all' istante di stop non sono piu' validi

    // se ho almeno un servente libero posso generare direttamente il suo servizio
    if (stato118 ->numberNode <= SERVERS_118) {
        double service  = GetService_118();
        int s = FindOne118(event_1);            // Trovo il server disponibile (idle) da piu' tempo

        sum_1[s].service += service;            // Aggiorno l'accumulo dei tempi di servizio al server s
        sum_1[s].served++;                      // Aumento il numero di jobs serviti dal server s

        event_1[s].t = current + service;       // Inserisco l'evento completamento del job al server s nella lista degli eventi,

                                                // Verrà processato in seguito
        event_1[s].x = 1;                       // imposto il server come busy

        //printf("Processing arrival at 118, completion at: %f\n", event_1[s].t);
    }

    // se non ho serventi liberi la chiamata viene rifiutata
    if(!IMPROVEMENT_MODEL && stato118 -> numberNode > SERVERS_118) {
        stato118 -> numberNode--;               // Diminuisco il numero di jobs nel sistema (aumentato a priori)
        //printf("[reject at server 1]\n");
        (*reject)++;                            // Aumento il numero di jobs rejected
    }
    else if (IMPROVEMENT_MODEL && stato118 -> numberNode > SERVERS_118) {
        stato118->numberQueue ++;
    }
}

int FindOne118(event_list_No_Priority event[] )
/* -----------------------------------------------------
 * return the index of the available server idle longest
 * (end index excluded)
 * -----------------------------------------------------
 */
{
    int s;
    int i = 1;

    while (event[i].x == 1)       /* find the index of the first available */
        i++;                        /* (idle) server                         */
    s = i;

    while (i < SERVERS_118) {         /* now, check the others to find which   */
        i++;                        /* has been idle longest                 */
        if ((event[i].x == 0) && (event[i].t < event[s].t))
        s = i;
    }
    return (s);
}


double GetService_118(){
    SelectStream(1);
    return (Exponential(1.35));
}


void SetupNode_118 (event_list_No_Priority* event,sum_server_No_Priority* sum){

    if(FINITE_HORIZON)
        event[0].t = GetArrival_118_BURST();
    else
        event[0].t   = GetArrival_118();
    event[0].x   = 1;

    for (int s = 1; s <= SERVERS_118; s++) {
        event[s].t     = START;          /* this value is arbitrary because */
        event[s].x     = 0;              /* all servers are initially idle  */
        sum[s].service = 0.0;
        sum[s].served  = 0;
    }
}
