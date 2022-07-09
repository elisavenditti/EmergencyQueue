#include <stdio.h>
#include <math.h>
#include "nodi.h"
#include "rngs.h"
#include "rvms.h"



void simulation_finite_horizon(int iterations, FILE* file_stats,statistics_mean* stats){

    long reject=0;
    long death = 0;
    long deathPS = 0;
    long deathPS_2 = 0;

    struct {
        double current;                  /* current time                       */
        double next;                     /* next (most imminent) event time    */
    } t = {0.0,0.0};

    int        e;                      /* next event index                   */
    int        s;                      /* server index                       */

    // FASE DI INIZIALIZZAZIONE
    double stop118 = 0.0;
    double stopAmbulance = 0.0;
    double stopAccettazione = 0.0;
    double stopAccettazione_2 = 0.0;
    double stopPS = 0.0;
    double stopPS_2 = 0.0;
    double nextSample = SAMPLE_TIME;
    int res[2] = {0,0};

    area area_server_118 = {0.0,0.0,0.0};
    area area_server_Ambulance = {0.0, 0.0, 0.0};
    area area_server_ProntoSoccorso = {0.0, 0.0, 0.0};
    area area_server_Accettazione = {0.0,0.0,0.0};
    area area_server_ProntoSoccorso_2 = {0.0, 0.0, 0.0};
    area area_server_Accettazione_2 = {0.0,0.0,0.0};

    stato118 stato118 = {0,0,0};
    statoAmbulance statoAmbulanza = {0,0,0,0,0};
    statoProntoSoccorso statoProntoSoccorso_2 = {0,0,0,0,0,0,0,0,0};
    statoAccettazione statoAccettazione_2 = {0,0,0};
    statoProntoSoccorso statoProntoSoccorso = {0,0,0,0,0,0,0,0,0};
    statoAccettazione statoAccettazione = {0,0,0};


    sum_server_No_Priority sum_118[SERVERS_118 + 1];
    sum_server_Priority sum_Ambulance[SERVERS_Ambulance + 1];
    sum_server_Priority sum_ProntoSoccorso[SERVERS_ProntoSoccorso + 2];
    sum_server_No_Priority sum_Accettazione[SERVERS_Accettazione + 1];
    sum_server_Priority sum_ProntoSoccorso_2[SERVERS_ProntoSoccorso_2 + 2];
    sum_server_No_Priority sum_Accettazione_2[SERVERS_Accettazione_2 + 1];


    event_list_No_Priority event_118[SERVERS_118 + 1];
    event_list_Priority event_Ambulance[SERVERS_Ambulance + 1];
    event_list_Priority event_ProntoSoccorso[SERVERS_ProntoSoccorso + 2]; //Arrivi da Ambulanze  + Accettazione
    event_list_No_Priority event_Accettazione[SERVERS_Accettazione + 1];
    event_list_Priority event_ProntoSoccorso_2[SERVERS_ProntoSoccorso_2 + 2]; //Arrivi da Ambulanze  + Accettazione
    event_list_No_Priority event_Accettazione_2[SERVERS_Accettazione_2 + 1];

    SetupNode_118(event_118,sum_118);
    SetupNode_Ambulance(event_Ambulance,sum_Ambulance);
    SetupNode_ProntoSoccorso(event_ProntoSoccorso,sum_ProntoSoccorso);
    SetupNode_Accettazione (event_Accettazione, sum_Accettazione);
    SetupNode_ProntoSoccorso_2(event_ProntoSoccorso_2,sum_ProntoSoccorso_2);
    SetupNode_Accettazione_2 (event_Accettazione_2, sum_Accettazione_2);


    while ((event_118[0].x != 0 || event_Ambulance[0].x != 0 || event_ProntoSoccorso[0].x != 0
          || event_ProntoSoccorso[1].x != 0 || event_Accettazione[0].x != 0 ||  event_ProntoSoccorso_2[0].x != 0
          || event_ProntoSoccorso_2[1].x != 0 || event_Accettazione_2[0].x != 0) ||
         (stato118.numberNode + statoAmbulanza.numberNode + statoProntoSoccorso.numberNode + statoAccettazione.numberNode
          + statoProntoSoccorso_2.numberNode + statoAccettazione_2.numberNode) != 0){

        int list_id;
        NextEvent(event_118, event_Ambulance, event_ProntoSoccorso, event_Accettazione, event_ProntoSoccorso_2,event_Accettazione_2, res);
        e = res[0];
        list_id = res[1];

        switch (list_id){
            case 1: t.next = event_118[e].t;
            break;

            case 2: t.next = event_Ambulance[e].t;
            break;

            case 3: t.next = event_ProntoSoccorso[e].t;
            break;

            case 4: t.next = event_Accettazione[e].t;
            break;

            case 5: t.next = event_Accettazione_2[e].t;
            break;

            case 6: t.next = event_ProntoSoccorso_2[e].t;
            break;

            default : break;
        }

        // update integral
        double delta_t = t.next - t.current;


        if(IMPROVEMENT_MODEL){
            area_server_118.node    += delta_t * stato118.numberNode;
            area_server_118.service += delta_t * stato118.numberNode - stato118.numberQueue;
            area_server_118.queue   += delta_t * stato118.numberQueue;
        } else {
            area_server_118.node    += delta_t * stato118.numberNode;
            area_server_118.service += delta_t * stato118.numberNode;
        }

        area_server_Ambulance.node    += delta_t * statoAmbulanza.numberNode;
        area_server_Ambulance.service += delta_t * (statoAmbulanza.numberNode - statoAmbulanza.numberQueue);
        area_server_Ambulance.queue   += delta_t * statoAmbulanza.numberQueue;

        area_server_ProntoSoccorso.node    += delta_t * statoProntoSoccorso.numberNode;
        area_server_ProntoSoccorso.service += delta_t * (statoProntoSoccorso.numberNode - statoProntoSoccorso.numberQueue);
        area_server_ProntoSoccorso.queue   += delta_t * statoProntoSoccorso.numberQueue;

        area_server_Accettazione.node    += delta_t * statoAccettazione.numberNode;
        area_server_Accettazione.service += delta_t * (statoAccettazione.numberNode - statoAccettazione.numberQueue);
        area_server_Accettazione.queue   += delta_t * statoAccettazione.numberQueue;

        area_server_ProntoSoccorso_2.node    += delta_t * statoProntoSoccorso_2.numberNode;
        area_server_ProntoSoccorso_2.service += delta_t * (statoProntoSoccorso_2.numberNode - statoProntoSoccorso_2.numberQueue);
        area_server_ProntoSoccorso_2.queue   += delta_t * statoProntoSoccorso_2.numberQueue;

        area_server_Accettazione_2.node    += delta_t * statoAccettazione_2.numberNode;
        area_server_Accettazione_2.service += delta_t * (statoAccettazione_2.numberNode - statoAccettazione_2.numberQueue);
        area_server_Accettazione_2.queue   += delta_t * statoAccettazione_2.numberQueue;


        if(t.next > nextSample && t.current < nextSample){
            int deltaSample = 1;
            if(t.next - t.current > SAMPLE_TIME){
                deltaSample = (t.next - t.current) / SAMPLE_TIME;
            }
            double stat_118 = 0.0;
            double mean_delay_ambulance = 0.0;
            double mean_delay_ps1=0.0;
            double mean_delay_ps2 = 0.0;
            double mean_delay_a1 = 0.0;
            double mean_delay_a2 = 0.0;

            if(IMPROVEMENT_MODEL && stato118.numberCompletion != 0) stat_118 = (area_server_118.queue - (t.next - nextSample*deltaSample)*stato118.numberQueue) / stato118.numberCompletion;
            else stat_118 = reject;

            if(statoAmbulanza.numberCompletion != 0)
                mean_delay_ambulance = (area_server_Ambulance.queue - (t.next - nextSample*deltaSample)*statoAmbulanza.numberQueue) / statoAmbulanza.numberCompletion;
            if(statoProntoSoccorso.numberCompletion != 0)
                mean_delay_ps1 = (area_server_ProntoSoccorso.queue- (t.next - nextSample*deltaSample)*statoProntoSoccorso.numberQueue) / statoProntoSoccorso.numberCompletion;
            if(statoProntoSoccorso_2.numberCompletion != 0)
                mean_delay_ps2 = (area_server_ProntoSoccorso_2.queue - (t.next - nextSample*deltaSample)*statoProntoSoccorso_2.numberQueue) / statoProntoSoccorso_2.numberCompletion;
            if(statoAccettazione.numberCompletion != 0)
                mean_delay_a1 = (area_server_Accettazione.queue - (t.next - nextSample*deltaSample)*statoAccettazione.numberQueue) / statoAccettazione.numberCompletion;
            if(statoAccettazione_2.numberCompletion != 0)
                mean_delay_a2 = (area_server_Accettazione_2.queue - (t.next - nextSample*deltaSample)*statoAccettazione_2.numberQueue) / statoAccettazione_2.numberCompletion;


            fprintf(file_stats, "%d,%f,%f,%f,%f,%f,%f\n",iterations,stat_118,mean_delay_ambulance, mean_delay_a1,
                    mean_delay_a2,mean_delay_ps1,mean_delay_ps2);
            nextSample += SAMPLE_TIME *deltaSample;
        }


        // avanza il clock
        t.current = t.next;

        // arrival to 118
        if (e == 0 && list_id == 1) {
            arrivalAt118(t.current, event_118, sum_118, &stato118, &reject);
        }
        // completion of 118
        else if (e != 0 && list_id == 1){
        completionAt118(t.current, event_118, event_Ambulance,&stato118, &statoAmbulanza,sum_118, sum_Ambulance,&stop118, e);
        }
        // completion of ambulance
        else if (e != 0 && list_id == 2){
            completionAtAmbulance(t.current, event_Ambulance, sum_Ambulance, &statoAmbulanza, &stato118, e);
            if(generateProntoSoccorso(statoProntoSoccorso,statoProntoSoccorso_2)) {
            // generate death potremmo portarlo fuori:
                if (generateDeath()) {
                    death ++;
                } else {
                    event_ProntoSoccorso[0].t = event_Ambulance[e].t;           //Aggiorno l'arrivo al sistema Pronto soccorso come fosse istantaneo al completamento del job al sistema ambulanze
                    //event_ProntoSoccorso[0].x = 1;                            //Arrivo valido
                    int type = GenerateType_From_Ambulance();
                    arrivalAtProntoSoccorso(t.current, type, event_ProntoSoccorso, sum_ProntoSoccorso, &statoProntoSoccorso, &deathPS);
                }
            } else {
                if (generateDeath()) {
                    death ++;
                } else {
                    event_ProntoSoccorso_2[0].t = event_Ambulance[e].t;           //Aggiorno l'arrivo al sistema Pronto soccorso come fosse istantaneo al completamento del job al sistema ambulanze
                    //event_ProntoSoccorso_2[0].x = 1;                            //Arrivo valido
                    int type = GenerateType_From_Ambulance();
                    arrivalAtProntoSoccorso_2(t.current, type, event_ProntoSoccorso_2, sum_ProntoSoccorso_2, &statoProntoSoccorso_2, &deathPS_2);
                }
            }

            // Se nel nodo ambulanze non ci sono piu' jobs da servire e non posso avere piu' arrivi alle ambulanze
            // significa che non posso piu' avere arrivi al nodo successivo
            if (event_Ambulance[0].x == 0 && statoAmbulanza.numberNode == 0){

                stopAmbulance = t.current;
                event_ProntoSoccorso[0].x = 0;
                event_ProntoSoccorso_2[0].x = 0;
            }

        }
        // completion of ER (pronto soccorso)
        else if (e != 0 && list_id == 3){
            completionAtProntoSoccorso(t.current, event_ProntoSoccorso, sum_ProntoSoccorso, &statoProntoSoccorso, &statoAmbulanza, &stopPS, e);
        }
        // arrival to triage (accettazione)
        else if (e == 0 && list_id == 4){
            arrivalAtAccettazione(t.current,event_Accettazione,sum_Accettazione, &statoAccettazione);
        }
        // completion of triage (accettazione)
        else if (e != 0 && list_id == 4) {
            completionAtAccettazione(t.current, event_Accettazione, event_ProntoSoccorso, &statoAccettazione, &statoProntoSoccorso, sum_Accettazione, sum_ProntoSoccorso, &stopAccettazione, e, &deathPS);
        }
        // completion of ER (pronto soccorso_2)
        else if (e != 0 && list_id == 6){
            completionAtProntoSoccorso_2(t.current, event_ProntoSoccorso_2, sum_ProntoSoccorso_2, &statoProntoSoccorso_2,&statoAmbulanza, &stopPS_2,e);
        }
        // arrival to triage (accettazione_2)
        else if (e == 0 && list_id == 5){
            arrivalAtAccettazione_2(t.current,event_Accettazione_2,sum_Accettazione_2, &statoAccettazione_2);
        }
        // completion of triage (accettazione_2)
        else if (e != 0 && list_id == 5) {
            completionAtAccettazione_2(t.current, event_Accettazione_2, event_ProntoSoccorso_2, &statoAccettazione_2, &statoProntoSoccorso_2, sum_Accettazione_2, sum_ProntoSoccorso_2, &stopAccettazione_2, e, &deathPS_2);
        }
    }

    if(stopPS == 0.0) stopPS = t.current;
    if(stopPS_2 == 0.0) stopPS_2 = t.current;

    if(!IMPROVEMENT_MODEL) stats->mean_loss_118 = (double) reject/ (stato118.numberCompletion + reject);
    else stats->mean_delay_118 = area_server_118.queue/ stato118.numberCompletion;
    stats->mean_utilization_118 = area_server_118.service/(stop118*SERVERS_118);

    stats->mean_delay_ambulance = area_server_Ambulance.queue / statoAmbulanza.numberCompletion;
    stats->mean_utilization_ambulance = area_server_Ambulance.service/(stopAmbulance*SERVERS_Ambulance);

    stats->mean_delay_pronto_soccorso1 = area_server_ProntoSoccorso.queue / statoProntoSoccorso.numberCompletion;
    stats->mean_utilization_ps1 = area_server_ProntoSoccorso.service/(stopPS*SERVERS_ProntoSoccorso);

    stats->mean_delay_pronto_soccorso2 = area_server_ProntoSoccorso_2.queue / statoProntoSoccorso_2.numberCompletion;
    stats->mean_utilization_ps2 = area_server_ProntoSoccorso_2.service/(stopPS_2*SERVERS_ProntoSoccorso_2);

    stats->mean_delay_accettazione1 = area_server_Accettazione.queue / statoAccettazione.numberCompletion;
    stats->mean_utilization_a1 = area_server_Accettazione.service/(stopAccettazione*SERVERS_Accettazione);

    stats->mean_delay_accettazione2 = area_server_Accettazione_2.queue / statoAccettazione_2.numberCompletion;
    stats->mean_utilization_a2 = area_server_Accettazione_2.service/(stopAccettazione_2*SERVERS_Accettazione_2);
    return ;
}
