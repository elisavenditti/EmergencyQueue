#include <stdio.h>
#include <math.h>
#include "rngs.h"
#include "nodi.h"



double GetArrival_Accettazione(){

    static double arrival = 0.0;
    if (restart[1]) {
        arrival = 0.0;
        restart[1] = 0;
    }
    SelectStream(7);
    arrival += Exponential(10.0);
    return (arrival);
}

int GenerateType_FromAccettazione(){
    // 2 rosso; 1 giallo; 0 verde
    SelectStream(9);
    if (Random() < 0.05)
        return (2);
    else if (Random() < 0.46)
        return (1);     //0.41 + 0.05
    else
        return (0);

}

double GetService_Accettazione(){
    SelectStream(8);
    return(Exponential(6.0));
}


void arrivalAtAccettazione(double current, event_list_No_Priority event_Accettazione[], sum_server_No_Priority sum_Accettazione[],
                           statoAccettazione* statoAccettazione){

    statoAccettazione -> numberNode++;
    event_Accettazione[0].t = GetArrival_Accettazione(); // genero il prossimo arrivo al sistema Accettazione

    /*printf("Processing arrival at Accettazione, next at: %f\n", event_Accettazione[0].t);
    printf("JOB IN ACCETTAZIONE: %d\n", statoAccettazione -> numberNode);*/

    if (event_Accettazione[0].t > STOP)
        event_Accettazione[0].x = 0;

    // se ho abbastanza serventi l'arrivo è immediatamente servito
    if (statoAccettazione-> numberNode <= SERVERS_Accettazione) {
        double service  = GetService_Accettazione();                    // Genero il servizio del job
        int  s = FindOneAccettazione(event_Accettazione);               // Trovo il server libero da piu' tempo
        sum_Accettazione[s].service += service;                         // Aggiungo il tempo di servizio alla somma dei precedenti tempi di servizio per il server s
        sum_Accettazione[s].served++;                                   // Aumento il numero di jobs serviti al server s

        event_Accettazione[s].t = current + service;                    // Genero il prossimo completamento al server s
        event_Accettazione[s].x= 1;                                     // Server s busy
        //printf("    entro in servizio nel servente (%d) libero, servizio a : %f\n", s, event_Accettazione[s].t);
    } else {
        // L'arrivo è aggiunto alla coda
        statoAccettazione->numberQueue++;
        //printf("    vado in coda\n");
    }
}


void completionAtAccettazione (double current, event_list_No_Priority event_Accettazione[], event_list_Priority event_ProntoSoccorso[],  statoAccettazione* statoAccettazione, statoProntoSoccorso* statoProntoSoccorso, sum_server_No_Priority sum_Accettazione[],
                               sum_server_Priority sum_ProntoSoccorso[], double* stopAccettazione,  int e, long* deathPS){


    statoAccettazione->numberNode--;                                  // Diminuisco il numero di jobs nel sistema
    statoAccettazione->numberCompletion++;                            // Aumento il numero di completamenti per l'accettazione

    //printf("Processing completion at Accettazione, simultaneous Pronto Soccorso arrival at: %f\n", event_Accettazione[e].t);

    int s = e;
    //printf("[SMALTISCO IL JOB NEL SERVENTE %d]\n",s);

    // se ho persone in coda le servo, NB: l'unico servente libero è quello appena liberato
    if (statoAccettazione->numberNode >= SERVERS_Accettazione) {
        statoAccettazione -> numberQueue --;

        double service   = GetService_Accettazione();

        sum_Accettazione[s].service += service;                             // Aumento l'accumulated service times per il server s
        sum_Accettazione[s].served++;                                       // Aumento il numero di jobs serviti dal server s

        event_Accettazione[s].t = current + service;                        // Aggiorno il prossimo completamento per il server s
        //printf("    servizio = %lf\n", event_Accettazione[s].t);

    } else event_Accettazione[s].x = 0;                                        // Imposto il server a idle

    // imposto il prossimo arrivo al pronto soccorso da parte dell'accettazione ???
    event_ProntoSoccorso[1].t = event_Accettazione[s].t;

    if (event_Accettazione[0].x == 0 && statoAccettazione->numberNode == 0){
        *stopAccettazione = current;
        event_ProntoSoccorso[1].x = 0;
    }
    int type = GenerateType_FromAccettazione();
    arrivalAtProntoSoccorso(current,type,event_ProntoSoccorso,sum_ProntoSoccorso, statoProntoSoccorso, deathPS);
}

void SetupNode_Accettazione (event_list_No_Priority* event,sum_server_No_Priority* sum){

    event[0].t   = GetArrival_Accettazione();
    event[0].x   = 1;

    for (int s = 1; s <  SERVERS_Accettazione + 1; s++) {
        event[s].t    = 0.0;
        event[s].x = 0;
        sum[s].service = 0.0;
        sum[s].served = 0.0;
    }
}

int FindOneAccettazione(event_list_No_Priority event[]){
  int s = -1;
  int i = 1;

  while (event[i].x == 1)       /* find the index of the first available */
    i++;                        /* (idle) server                         */
  s = i;

  while (i < SERVERS_Accettazione) {         /* now, check the others to find which   */
    i++;                        /* has been idle longest                 */
    if ((event[i].x == 0) && (event[i].t < event[s].t))
      s = i;
  }
  return (s);
}




