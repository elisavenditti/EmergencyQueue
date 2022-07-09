#include <stdio.h>
#include <math.h>
#include "nodi.h"
#include "rngs.h"
#include "rvms.h"


#define batch_size 512

void reset_stats_stato(stato118* stato118, statoAmbulance* statoAmbulance, statoProntoSoccorso* statoP1,
    statoProntoSoccorso* statoP2, statoAccettazione* statoA1, statoAccettazione* statoA2){

    stato118->numberCompletion = 0;
    statoA1->numberCompletion = 0;
    statoA2->numberCompletion = 0;
    statoP1->numberCompletion = 0;
    statoP2->numberCompletion = 0;
    statoAmbulance->numberCompletion = 0;
}


void reset_stats_area(area* areaStato){
    areaStato->node = 0;
    areaStato->queue = 0;
    areaStato->service=0;
}

 void update_variance(statistics_variance* stats_variance, statistics_mean stats, double nBatch,double mean_delay_118, double mean_utilization_118, double mean_delay_ambulance_batch,
                  double mean_utilization_ambulance, double mean_delay_ps1_batch,double mean_utilization_ps1,
                  double mean_delay_ps2_batch, double mean_utilization_ps2, double mean_delay_a1_batch,double mean_utilization_a1,
                  double mean_delay_a2_batch,double mean_utilization_a2, int reject){

    double aux = (nBatch-1) /  nBatch;

    if(!IMPROVEMENT_MODEL)stats_variance->variance_loss_118 += aux* pow((reject - stats.mean_loss_118),2);
    else stats_variance->variance_delay_118 += aux* pow((mean_delay_118 - stats.mean_delay_118),2);
    stats_variance->variance_utilization_118 += aux*pow((mean_utilization_118 - stats.mean_utilization_118),2);

    stats_variance->variance_delay_ambulance +=  aux* pow((mean_delay_ambulance_batch - stats.mean_delay_ambulance),2);
    stats_variance->variance_utilization_ambulance += aux*pow((mean_utilization_ambulance - stats.mean_utilization_ambulance),2);

    stats_variance->variance_delay_pronto_soccorso1 += aux*pow((mean_delay_ps1_batch - stats.mean_delay_pronto_soccorso1),2);
    stats_variance->variance_utilization_ps1 += aux*pow((mean_utilization_ps1 - stats.mean_utilization_ps1),2);

    stats_variance->variance_delay_pronto_soccorso2 += aux*pow((mean_delay_ps2_batch -  stats.mean_delay_pronto_soccorso2),2);
    stats_variance->variance_utilization_ps2 += aux*pow((mean_utilization_ps2 - stats.mean_utilization_ps2),2);

    stats_variance->variance_delay_accettazione1 += aux*pow((mean_delay_a1_batch - stats.mean_delay_accettazione1),2);
    stats_variance->variance_utilization_a1 += aux*pow((mean_utilization_a1 - stats.mean_utilization_a1),2);

    stats_variance->variance_delay_accettazione2 +=  aux*pow((mean_delay_a2_batch -  stats.mean_delay_accettazione2),2);
    stats_variance->variance_utilization_a2 += aux*pow((mean_utilization_a2 - stats.mean_utilization_a2),2);

 }

 void update_mean(statistics_mean* stats, double nBatch,double mean_delay_118, double mean_utilization_118, double mean_delay_ambulance_batch,
                  double mean_utilization_ambulance, double mean_delay_ps1_batch,double mean_utilization_ps1,
                  double mean_delay_ps2_batch, double mean_utilization_ps2, double mean_delay_a1_batch,double mean_utilization_a1,
                  double mean_delay_a2_batch,double mean_utilization_a2, int reject){

    if(!IMPROVEMENT_MODEL)stats->mean_loss_118 += (reject - stats->mean_loss_118)/ (nBatch);
    else stats->mean_delay_118 += (mean_delay_118 - stats->mean_delay_118)/(nBatch);
    stats->mean_utilization_118 += (mean_utilization_118 - stats->mean_utilization_118) /(nBatch);

    stats->mean_delay_ambulance +=  (mean_delay_ambulance_batch - stats->mean_delay_ambulance)/ (nBatch);
    stats->mean_utilization_ambulance += (mean_utilization_ambulance - stats->mean_utilization_ambulance) /(nBatch);

    stats->mean_delay_pronto_soccorso1 += (mean_delay_ps1_batch - stats->mean_delay_pronto_soccorso1)/ (nBatch);
    stats->mean_utilization_ps1 += (mean_utilization_ps1 - stats->mean_utilization_ps1) /(nBatch);

    stats->mean_delay_pronto_soccorso2 += (mean_delay_ps2_batch - stats->mean_delay_pronto_soccorso2)/ (nBatch);
    stats->mean_utilization_ps2 += (mean_utilization_ps2 - stats->mean_utilization_ps2) /(nBatch);

    stats->mean_delay_accettazione1 +=  (mean_delay_a1_batch - stats->mean_delay_accettazione1)/ (nBatch);
    stats->mean_utilization_a1 += (mean_utilization_a1 - stats->mean_utilization_a1) /(nBatch);

    stats->mean_delay_accettazione2 += (mean_delay_a2_batch - stats->mean_delay_accettazione2)/ (nBatch);
    stats->mean_utilization_a2 += (mean_utilization_a2 - stats->mean_utilization_a2) /(nBatch);
}

void simulation_infinite_horizon(void){

    FILE *file_stats;
    file_stats = fopen("./infiniteHorizonStatistics.txt", "w+");

    long reject=0;
    long death = 0;
    long deathPS = 0;
    long deathPS_2 = 0;
    double totalAverageNQPS1 = 0.0;

    struct {
        double current;                  /* current time                       */
        double next;                     /* next (most imminent) event time    */
        double batch;
    } t;

    int e;                      /* next event index                   */
    int s;                      /* server index                       */

    // FASE DI INIZIALIZZAZIONE
    double stop118 = 0.0;
    double stopAmbulance = 0.0;
    double stopAccettazione = 0.0;
    double stopAccettazione_2 = 0.0;
    double stopPS = 0.0;
    double stopPS_2 = 0.0;
    double mean_delay_118 = 0.0;

    int nJobInBatch = 0;
    double nBatch = 1.0;
    int total_nCompletion [6] = {0,0,0,0,0,0};

    statistics_mean stats = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    statistics_variance stats_variance = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    int res[2];
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

    PlantSeeds(123456789);
    t.current = START;
    t.batch = START;
    t.next = START;

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
          + statoProntoSoccorso_2.numberNode + statoAccettazione_2.numberNode) != 0) {

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

        // avanza il clock
        t.current = t.next;

        // arrival to 118
        if (e == 0 && list_id == 1) {
            arrivalAt118(t.current, event_118, sum_118, &stato118, &reject);
        }
        // completion of 118
        else if (e != 0 && list_id == 1){
        completionAt118(t.current, event_118, event_Ambulance,&stato118, &statoAmbulanza, sum_118, sum_Ambulance,&stop118, e);
        }
        // completion of ambulance
        else if (e != 0 && list_id == 2){
            completionAtAmbulance(t.current, event_Ambulance, sum_Ambulance, &statoAmbulanza, &stato118, e);
            if(generateProntoSoccorso(statoProntoSoccorso, statoProntoSoccorso_2)) {
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

        if (e == 0){
            nJobInBatch++;
            if(nJobInBatch % batch_size == 0){
                stop118= t.current - t.batch;
                stopAccettazione = t.current - t.batch;
                stopAccettazione_2 = t.current - t.batch;
                stopAmbulance = t.current - t.batch;
                stopPS = t.current - t.batch;
                stopPS_2 = t.current - t.batch;

                if(IMPROVEMENT_MODEL) mean_delay_118 = area_server_118.queue / stato118.numberCompletion;
                double mean_utilization_118 = area_server_118.service/(stop118*SERVERS_118);

                double mean_delay_ambulance_batch = area_server_Ambulance.queue / statoAmbulanza.numberCompletion;
                double mean_utilization_ambulance = area_server_Ambulance.service/(stopAmbulance*SERVERS_Ambulance);

                double mean_delay_ps1_batch = area_server_ProntoSoccorso.queue / statoProntoSoccorso.numberCompletion;
                double mean_utilization_ps1 = area_server_ProntoSoccorso.service/(stopPS*SERVERS_ProntoSoccorso);
                double averageNumberInQueuePS1 = area_server_ProntoSoccorso.queue/stopPS;

                double mean_delay_ps2_batch = area_server_ProntoSoccorso_2.queue / statoProntoSoccorso_2.numberCompletion;
                double mean_utilization_ps2 = area_server_ProntoSoccorso_2.service/(stopPS_2*SERVERS_ProntoSoccorso_2);

                double mean_delay_a1_batch = area_server_Accettazione.queue / statoAccettazione.numberCompletion;
                double mean_utilization_a1 = area_server_Accettazione.service/(stopAccettazione*SERVERS_Accettazione);

                double mean_delay_a2_batch = area_server_Accettazione_2.queue / statoAccettazione_2.numberCompletion;
                double mean_utilization_a2 = area_server_Accettazione_2.service/(stopAccettazione_2*SERVERS_Accettazione_2);


                update_variance(&stats_variance,stats, nBatch, mean_delay_118, mean_utilization_118,mean_delay_ambulance_batch, mean_utilization_ambulance,
                          mean_delay_ps1_batch,mean_utilization_ps1, mean_delay_ps2_batch, mean_utilization_ps2,mean_delay_a1_batch, mean_utilization_a1, mean_delay_a2_batch,mean_utilization_a2, reject);


                update_mean (&stats, nBatch,mean_delay_118,mean_utilization_118,mean_delay_ambulance_batch, mean_utilization_ambulance,
                          mean_delay_ps1_batch,mean_utilization_ps1, mean_delay_ps2_batch, mean_utilization_ps2,mean_delay_a1_batch, mean_utilization_a1, mean_delay_a2_batch,mean_utilization_a2, reject);

                totalAverageNQPS1 += (averageNumberInQueuePS1-totalAverageNQPS1)/(nBatch);


                total_nCompletion[0] = total_nCompletion[0] + stato118.numberCompletion;
                total_nCompletion[1] = total_nCompletion[1] + statoAmbulanza.numberCompletion;
                total_nCompletion[2] = total_nCompletion[2] + statoAccettazione.numberCompletion;
                total_nCompletion[3] = total_nCompletion[3] + statoAccettazione_2.numberCompletion;
                total_nCompletion[4] = total_nCompletion[4] + statoProntoSoccorso.numberCompletion;
                total_nCompletion[5] = total_nCompletion[5] + statoProntoSoccorso_2.numberCompletion;

                reset_stats_stato(&stato118, &statoAmbulanza, &statoProntoSoccorso, &statoProntoSoccorso_2, &statoAccettazione, &statoAccettazione_2);
                reset_stats_area(&area_server_118);
                reset_stats_area(&area_server_Ambulance);
                reset_stats_area(&area_server_ProntoSoccorso);
                reset_stats_area(&area_server_ProntoSoccorso_2);
                reset_stats_area(&area_server_Accettazione);
                reset_stats_area(&area_server_Accettazione_2);

                double criticalValue = idfStudent(nBatch-1,1-0.025); //alpha = 0.05
                double conf_mean_delay_118 = (criticalValue*sqrt(stats_variance.variance_delay_118 / nBatch))/sqrt(nBatch-1);
                double conf_mean_delay_ambulance = (criticalValue*sqrt(stats_variance.variance_delay_ambulance / nBatch))/sqrt(nBatch-1);
                double conf_mean_loss_118 = (criticalValue*sqrt(stats_variance.variance_loss_118 / nBatch))/sqrt(nBatch-1);
                double conf_mean_delay_ps1 = (criticalValue*sqrt(stats_variance.variance_delay_pronto_soccorso1/nBatch))/sqrt(nBatch-1);
                double conf_mean_delay_ps2 = (criticalValue*sqrt(stats_variance.variance_delay_pronto_soccorso2/nBatch))/sqrt(nBatch-1);
                double conf_mean_delay_a1 = (criticalValue*sqrt(stats_variance.variance_delay_accettazione1/nBatch))/sqrt(nBatch-1);
                double conf_mean_delay_a2 = (criticalValue*sqrt(stats_variance.variance_delay_accettazione2/nBatch))/sqrt(nBatch-1);

                if(nBatch == 1 && !IMPROVEMENT_MODEL){
                    fprintf(file_stats, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", nBatch,stats.mean_loss_118,0.0,
                        stats.mean_delay_ambulance,0.0, stats.mean_delay_accettazione1,0.0,
                        stats.mean_delay_accettazione2,0.0,stats.mean_delay_pronto_soccorso1,0.0,
                        stats.mean_delay_pronto_soccorso2, 0.0);
                }
                else if (!IMPROVEMENT_MODEL){
                    fprintf(file_stats, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", nBatch,stats.mean_loss_118,conf_mean_loss_118,
                        stats.mean_delay_ambulance,conf_mean_delay_ambulance, stats.mean_delay_accettazione1,conf_mean_delay_a1,
                        stats.mean_delay_accettazione2,conf_mean_delay_a2,stats.mean_delay_pronto_soccorso1,conf_mean_delay_ps1,
                        stats.mean_delay_pronto_soccorso2,conf_mean_delay_ps2);
                }

                if(nBatch == 1 && IMPROVEMENT_MODEL){
                    fprintf(file_stats, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", nBatch,stats.mean_delay_118,0.0,
                        stats.mean_delay_ambulance,0.0, stats.mean_delay_accettazione1,0.0,
                        stats.mean_delay_accettazione2,0.0,stats.mean_delay_pronto_soccorso1,0.0,
                        stats.mean_delay_pronto_soccorso2, 0.0);
                }
                else if (IMPROVEMENT_MODEL){
                    fprintf(file_stats, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", nBatch,stats.mean_delay_118,conf_mean_delay_118,
                        stats.mean_delay_ambulance,conf_mean_delay_ambulance, stats.mean_delay_accettazione1,conf_mean_delay_a1,
                        stats.mean_delay_accettazione2,conf_mean_delay_a2,stats.mean_delay_pronto_soccorso1,conf_mean_delay_ps1,
                        stats.mean_delay_pronto_soccorso2,conf_mean_delay_ps2);
                }
                reject = 0;
                stop118 = 0.0;
                stopAccettazione = 0.0;
                stopAccettazione_2 = 0.0;
                stopAmbulance = 0.0;
                stopPS = 0.0;
                stopPS_2 = 0.0;
                nBatch++;
                t.batch = t.current;
            }
        }
    }
    if(stopPS == 0.0) stopPS = t.current;
    if(stopPS_2 == 0.0) stopPS_2 = t.current;

    stop118 -=  t.batch;
    stopAccettazione -=  t.batch;
    stopAccettazione_2 -= t.batch;
    stopAmbulance -=  t.batch;
    stopPS -=  t.batch;
    stopPS_2 -=  t.batch;

    if(IMPROVEMENT_MODEL) mean_delay_118 = area_server_118.queue / stato118.numberCompletion;
    double mean_utilization_118 = area_server_118.service/(stop118*SERVERS_118);

    double mean_delay_ambulance_batch = area_server_Ambulance.queue / statoAmbulanza.numberCompletion;
    double mean_utilization_ambulance = area_server_Ambulance.service/(stopAmbulance*SERVERS_Ambulance);

    double mean_delay_ps1_batch = area_server_ProntoSoccorso.queue / statoProntoSoccorso.numberCompletion;
    double mean_utilization_ps1 = area_server_ProntoSoccorso.service/(stopPS*SERVERS_ProntoSoccorso);
    double averageNumberInQueuePS1 = area_server_ProntoSoccorso.queue/stopPS;

    double mean_delay_ps2_batch = area_server_ProntoSoccorso_2.queue / statoProntoSoccorso_2.numberCompletion;
    double mean_utilization_ps2 = area_server_ProntoSoccorso_2.service/(stopPS_2*SERVERS_ProntoSoccorso_2);

    double mean_delay_a1_batch = area_server_Accettazione.queue / statoAccettazione.numberCompletion;
    double mean_utilization_a1 = area_server_Accettazione.service/(stopAccettazione*SERVERS_Accettazione);

    double mean_delay_a2_batch = area_server_Accettazione_2.queue / statoAccettazione_2.numberCompletion;
    double mean_utilization_a2 = area_server_Accettazione_2.service/(stopAccettazione_2*SERVERS_Accettazione_2);

    update_variance(&stats_variance,stats,nBatch,mean_delay_118,mean_utilization_118,mean_delay_ambulance_batch, mean_utilization_ambulance,
                          mean_delay_ps1_batch,mean_utilization_ps1, mean_delay_ps2_batch, mean_utilization_ps2,mean_delay_a1_batch, mean_utilization_a1, mean_delay_a2_batch,mean_utilization_a2, reject);

    if(!IMPROVEMENT_MODEL) stats_variance.variance_loss_118 = stats_variance.variance_loss_118 / nBatch;
    else stats_variance.variance_delay_118 = stats_variance.variance_delay_118 /nBatch;

    stats_variance.variance_utilization_118 = stats_variance.variance_utilization_118 / nBatch;
    stats_variance.variance_delay_ambulance = stats_variance.variance_delay_ambulance / nBatch;
    stats_variance.variance_utilization_ambulance = stats_variance.variance_utilization_ambulance / nBatch;

    stats_variance.variance_delay_accettazione1 = stats_variance.variance_delay_accettazione1 / nBatch;
    stats_variance.variance_utilization_a1 = stats_variance.variance_utilization_a1 / nBatch;

    stats_variance.variance_delay_accettazione2 = stats_variance.variance_delay_accettazione2 / nBatch;
    stats_variance.variance_utilization_a2 = stats_variance.variance_utilization_a2 / nBatch;

    stats_variance.variance_delay_pronto_soccorso1 = stats_variance.variance_delay_pronto_soccorso1 / nBatch;
    stats_variance.variance_utilization_ps1 = stats_variance.variance_utilization_ps1 / nBatch;

    stats_variance.variance_delay_pronto_soccorso2 = stats_variance.variance_delay_pronto_soccorso2 / nBatch;
    stats_variance.variance_utilization_ps2 = stats_variance.variance_utilization_ps2 / nBatch;

    update_mean (&stats, nBatch,mean_delay_118,mean_utilization_118,mean_delay_ambulance_batch, mean_utilization_ambulance,
                        mean_delay_ps1_batch,mean_utilization_ps1, mean_delay_ps2_batch, mean_utilization_ps2,mean_delay_a1_batch, mean_utilization_a1, mean_delay_a2_batch,mean_utilization_a2, reject);

    totalAverageNQPS1 += (averageNumberInQueuePS1-totalAverageNQPS1)/(nBatch);
    //printf("media totale,%f,%f,%f,%f,%f\n",stats.mean_delay_ambulance, stats.mean_delay_pronto_soccorso1,stats.mean_delay_pronto_soccorso2,
    //        stats.mean_delay_accettazione1,stats.mean_delay_accettazione2);

    //  printf("\n|******************* FINE CALCOLO AUTOCORRELATION LAG 1 *******************|\n");

    double criticalValue = idfStudent(nBatch-1,1-0.025); //alpha = 0.05


    printf("*\n* MAIN PERFORMANCE:\n");
    if(!IMPROVEMENT_MODEL) printf("* Mean loss: %lf +_ %lf \n", stats.mean_loss_118, (criticalValue*sqrt(stats_variance.variance_loss_118))/sqrt(nBatch-1));
    else printf("* Mean delay 118: %lf +_ %lf \n", stats.mean_delay_118, (criticalValue*sqrt(stats_variance.variance_delay_118))/sqrt(nBatch-1));
    printf("* Utilization system 118: %f +_ %f \n",stats.mean_utilization_118, (criticalValue*sqrt(stats_variance.variance_utilization_118))/sqrt(nBatch-1));

    printf("* Mean delay ambulance: %lf +_ %lf \n", stats.mean_delay_ambulance,(criticalValue*sqrt(stats_variance.variance_delay_ambulance))/sqrt(nBatch-1));
    printf("* Utilization system ambulance: %lf +_ %lf \n",stats.mean_utilization_ambulance, (criticalValue*sqrt(stats_variance.variance_utilization_ambulance))/sqrt(nBatch-1));

    printf("* Mean delay pronto soccorso: %lf +_ %lf \n", stats.mean_delay_pronto_soccorso1,(criticalValue*sqrt(stats_variance.variance_delay_pronto_soccorso1))/sqrt(nBatch-1));
    printf("* Utilization system pronto soccorso: %lf +_ %lf \n",stats.mean_utilization_ps1, (criticalValue*sqrt(stats_variance.variance_utilization_ps1))/sqrt(nBatch-1));

    printf("* Mean delay pronto soccorso 2: %lf +_ %lf \n", stats.mean_delay_pronto_soccorso2, (criticalValue*sqrt(stats_variance.variance_delay_pronto_soccorso2))/sqrt(nBatch-1));
    printf("* Utilization system pronto soccorso 2: %lf +_ %lf \n",stats.mean_utilization_ps2, (criticalValue*sqrt(stats_variance.variance_utilization_ps2))/sqrt(nBatch-1));

    printf("* Mean delay accettazione 1: %lf +_ %lf \n", stats.mean_delay_accettazione1, (criticalValue*sqrt(stats_variance.variance_delay_accettazione1))/sqrt(nBatch-1));
    printf("* Utilization system accettazione 1: %lf +_ %lf \n",stats.mean_utilization_a1, (criticalValue*sqrt(stats_variance.variance_utilization_a1))/sqrt(nBatch-1));

    printf("* Mean delay accettazione 2: %lf +_ %lf \n", stats.mean_delay_accettazione2, (criticalValue*sqrt(stats_variance.variance_delay_accettazione2))/sqrt(nBatch-1));
    printf("* Utilization system accettazione 2: %lf +_ %lf \n",stats.mean_utilization_a2, (criticalValue*sqrt(stats_variance.variance_utilization_a2))/sqrt(nBatch-1));
    printf("*\n\n\n");


    double lastMean118;
    double lastVariance118;

    if(!IMPROVEMENT_MODEL){
        lastMean118 = stats.mean_loss_118;
        lastVariance118 = stats_variance.variance_loss_118;
    } else {
        lastMean118 = stats.mean_delay_118;
        lastVariance118 = stats_variance.variance_delay_118;
    }

    fprintf(file_stats, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", nBatch, lastMean118,(criticalValue*sqrt(lastVariance118))/sqrt(nBatch-1),
        stats.mean_delay_ambulance,(criticalValue*sqrt(stats_variance.variance_delay_ambulance))/sqrt(nBatch-1),
        stats.mean_delay_accettazione1, (criticalValue*sqrt(stats_variance.variance_delay_accettazione1))/sqrt(nBatch-1),
        stats.mean_delay_accettazione2, (criticalValue*sqrt(stats_variance.variance_delay_accettazione2))/sqrt(nBatch-1),
        stats.mean_delay_pronto_soccorso1, (criticalValue*sqrt(stats_variance.variance_delay_pronto_soccorso1))/sqrt(nBatch-1),
        stats.mean_delay_pronto_soccorso2, (criticalValue*sqrt(stats_variance.variance_delay_pronto_soccorso2))/sqrt(nBatch-1));

    printf("*\n* NUMBER OF JOB PROCESSED:\n");
    printf("* 118: %ld job\n", stato118.numberCompletion + total_nCompletion[0]);
    printf("* Ambulance: %ld job\n", statoAmbulanza.numberCompletion + total_nCompletion[1]);
    printf("* -> %ld passing during transport to the hospital\n", death );
    printf("* Triage 1: %ld job\n", statoAccettazione.numberCompletion + total_nCompletion[2]);
    printf("* Triage 2: %ld job\n", statoAccettazione_2.numberCompletion + total_nCompletion[3]);
    printf("* ER 1: %ld job\n", statoProntoSoccorso.numberCompletion + total_nCompletion[4]);
    printf("* -> %f avg job in queue\n", totalAverageNQPS1);
    printf("* -> %ld patients with red code", statoProntoSoccorso.number_Red_Completion );
    printf("* -> %ld patients with yellow code\n", statoProntoSoccorso.number_Yellow_Completion );
    printf("* -> %ld patients with green code\n", statoProntoSoccorso.numberCompletion + total_nCompletion[4] -
            statoProntoSoccorso.number_Red_Completion -  statoProntoSoccorso.number_Yellow_Completion );
    printf("* -> %ld patients passed waiting the service\n", deathPS );
    printf("* ER 2: %ld job\n", statoProntoSoccorso_2.numberCompletion + total_nCompletion[5]);
    printf("* -> %ld patients with red code\n", statoProntoSoccorso_2.number_Red_Completion );
    printf("* -> %ld patients with yellow code\n", statoProntoSoccorso_2.number_Yellow_Completion );
    printf("* -> %ld patients with green code\n", statoProntoSoccorso_2.numberCompletion + total_nCompletion[5] -
        statoProntoSoccorso_2.number_Red_Completion -  statoProntoSoccorso_2.number_Yellow_Completion );
    printf("* -> %ld patients passed waiting the service\n", deathPS_2 );

    printf("*\n\n\n");

    printf("SERVER PERFORMANCE:\n\n");
    //118 server statistics
    printf(" 118 server statistics are:\n\n");
    printf("    server     utilization     avg service        share\n");
    for (s = 1; s <= SERVERS_118; s++)
        printf("%8d %14.3f %15.2f %15.3f\n", s, sum_118[s].service / event_118[s].t,
            sum_118[s].service / sum_118[s].served,
            (double) sum_118[s].served / (stato118.numberCompletion + total_nCompletion[0]));
    printf("\n");

    //ambulance server statistics
    printf(" Ambulance server statistics are:\n\n");
    printf("    server     utilization     avg service        share\n");
    for (s = 1; s <= SERVERS_Ambulance; s++)
        printf("%8d %14.3f %15.2f %15.3f\n", s, sum_Ambulance[s].service /event_Ambulance[s].t,
                sum_Ambulance[s].service / sum_Ambulance[s].served, (double) sum_Ambulance[s].served / (statoAmbulanza.numberCompletion + total_nCompletion[1]));
    printf("\n");


    //triage server statistics
    printf(" Triage 1 server statistics are:\n\n");
    printf("    server     utilization     avg service        share\n");
    for (s = 1; s <= SERVERS_Accettazione; s++)
        printf("%8d %14.3f %15.2f %15.3f\n", s, sum_Accettazione[s].service / event_Accettazione[s].t,
                sum_Accettazione[s].service / sum_Accettazione[s].served, (double) sum_Accettazione[s].served / (statoAccettazione.numberCompletion +  total_nCompletion[2]));
    printf("\n");

    //triage 2 server statistics
    printf(" Triage 2 server statistics are:\n\n");
    printf("    server     utilization     avg service        share\n");
    for (s = 1; s <= SERVERS_Accettazione_2; s++)
        printf("%8d %14.3f %15.2f %15.3f\n", s, sum_Accettazione_2[s].service / event_Accettazione_2[s].t,
            sum_Accettazione_2[s].service / sum_Accettazione_2[s].served, (double) sum_Accettazione_2[s].served / (statoAccettazione_2.numberCompletion + total_nCompletion[3]));
    printf("\n");


    //Emergency Room 1 server statistics
    printf(" Emergency Room 1 server statistics are:\n\n");
    printf("    server     utilization     avg service        share\n");
    for (s = 2; s <= SERVERS_ProntoSoccorso + 1; s++)
        printf("%8d %14.3f %15.2f %15.3f\n", s - 1, sum_ProntoSoccorso[s].service / event_ProntoSoccorso[s].t,
            sum_ProntoSoccorso[s].service / sum_ProntoSoccorso[s].served, (double) sum_ProntoSoccorso[s].served / (statoProntoSoccorso.numberCompletion + total_nCompletion[4]));
    printf("\n");



    //Emergency Room 2 server statistics
    printf(" Emergency Room 2 server statistics are:\n\n");
    printf("    server     utilization     avg service        share\n");
    for (s = 2; s <= SERVERS_ProntoSoccorso_2 + 1; s++)
        printf("%8d %14.3f %15.2f %15.3f\n", s - 1, sum_ProntoSoccorso_2[s].service / event_ProntoSoccorso[s].t,
            sum_ProntoSoccorso_2[s].service / sum_ProntoSoccorso_2[s].served, (double) sum_ProntoSoccorso_2[s].served / (statoProntoSoccorso_2.numberCompletion + total_nCompletion[5]));
    printf("\n");

    fclose(file_stats);

    return ;
}
