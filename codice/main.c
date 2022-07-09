#include <stdio.h>
#include <math.h>
#include "rngs.h"
#include "nodi.h"
#include "rvms.h"

#define ITERATIONS 64

int restart[3] = {1,1,1};

int main(void){

    statistics_mean stats_mean = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    statistics_variance stats_variance = {0.0,0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    statistics_mean actual_stats = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    printf(" _____                                                   ___\n");
    printf("| ____|_ __ ___   ___ _ __ __ _  ___ _ __   ___ _   _   / _ \\ _   _  ___ _   _  ___\n");
    printf("|  _| | '_ ` _ \\ / _ \\ '__/ _` |/ _ \\ '_ \\ / __| | | | | | | | | | |/ _ \\ | | |/ _ \\\n");
    printf("| |___| | | | | |  __/ | | (_| |  __/ | | | (__| |_| | | |_| | |_| |  __/ |_| |  __/\n");
    printf("|_____|_| |_| |_|\\___|_|  \\__, |\\___|_| |_|\\___|\\__, |  \\__\\\\_\\\__,_|\\___|\\__,_|\\___|\n");
    printf("                          |___/                 |___/                               \n\n");
    printf("                          Venditti Elisa - Zarrelli Ludovico\n\n");
    char* simulation;
    char* model;
    if(FINITE_HORIZON) {simulation = "finite horizon";}
    else{simulation = "infinite horizon";}
    if(!IMPROVEMENT_MODEL) {model = "basic algorithm";}
    else{model = "improved algorithm";}
    printf("                          SIMULATION: %s\n                          MODEL: %s\n\n", simulation, model);



    if(FINITE_HORIZON) {

        FILE *file_stats;
        file_stats = fopen("./finiteHorizonStatistics.txt", "w+");

        FILE *file_stats_samples;
        file_stats_samples = fopen("./finiteHorizonSamples.txt", "w+");

        PlantSeeds(123456789);
        for (int i = 1; i <= ITERATIONS; i++){
            restart[0] = 1;
            restart[1] = 1;
            restart[2] = 1;

            simulation_finite_horizon(i,file_stats_samples, &actual_stats);
            double aux = (i-1) /  (double) i;

            if(!IMPROVEMENT_MODEL)stats_variance.variance_loss_118 += aux* pow((actual_stats.mean_loss_118 - stats_mean.mean_loss_118),2);
            else stats_variance.variance_delay_118 += aux* pow((actual_stats.mean_delay_118 - stats_mean.mean_delay_118),2);
            stats_variance.variance_utilization_118 += aux* pow((actual_stats.mean_utilization_118 - stats_mean.mean_utilization_118),2);

            stats_variance.variance_delay_ambulance +=  aux* pow((actual_stats.mean_delay_ambulance - stats_mean.mean_delay_ambulance),2);
            stats_variance.variance_utilization_ambulance += aux* pow((actual_stats.mean_utilization_ambulance - stats_mean.mean_utilization_ambulance),2);

            stats_variance.variance_delay_accettazione1 += aux*pow((actual_stats.mean_delay_accettazione1 - stats_mean.mean_delay_accettazione1),2);
            stats_variance.variance_utilization_a1 += aux* pow((actual_stats.mean_utilization_a1 - stats_mean.mean_utilization_a1),2);

            stats_variance.variance_delay_accettazione2 += aux*pow((actual_stats.mean_delay_accettazione2 -  stats_mean.mean_delay_accettazione2),2);
            stats_variance.variance_utilization_a2 += aux* pow((actual_stats.mean_utilization_a2 - stats_mean.mean_utilization_a2),2);

            stats_variance.variance_delay_pronto_soccorso1 += aux*pow((actual_stats.mean_delay_pronto_soccorso1 - stats_mean.mean_delay_pronto_soccorso1),2);
            stats_variance.variance_utilization_ps1 +=  aux* pow((actual_stats.mean_utilization_ps1 - stats_mean.mean_utilization_ps1),2);

            stats_variance.variance_delay_pronto_soccorso2 +=  aux*pow((actual_stats.mean_delay_pronto_soccorso2-  stats_mean.mean_delay_pronto_soccorso2),2);
            stats_variance.variance_utilization_ps2 += aux* pow((actual_stats.mean_utilization_ps2 - stats_mean.mean_utilization_ps2),2);

            if(!IMPROVEMENT_MODEL) stats_mean.mean_loss_118 += (actual_stats.mean_loss_118 - stats_mean.mean_loss_118)/ (i);
            else stats_mean.mean_delay_118 += (actual_stats.mean_delay_118 - stats_mean.mean_delay_118)/(i);
            stats_mean.mean_utilization_118 += (actual_stats.mean_utilization_118 - stats_mean.mean_utilization_118) /(i);

            stats_mean.mean_delay_ambulance += (actual_stats.mean_delay_ambulance - stats_mean.mean_delay_ambulance)/ (i);
            stats_mean.mean_utilization_ambulance += (actual_stats.mean_utilization_ambulance - stats_mean.mean_utilization_ambulance) /(i);

            stats_mean.mean_delay_accettazione1 += (actual_stats.mean_delay_accettazione1 - stats_mean.mean_delay_accettazione1)/ (i);
            stats_mean.mean_utilization_a1 += (actual_stats.mean_utilization_a1 - stats_mean.mean_utilization_a1) /(i);

            stats_mean.mean_delay_accettazione2 += (actual_stats.mean_delay_accettazione2 - stats_mean.mean_delay_accettazione2)/ (i);
            stats_mean.mean_utilization_a2 += (actual_stats.mean_utilization_a2 - stats_mean.mean_utilization_a2) /(i);

            stats_mean.mean_delay_pronto_soccorso1 += (actual_stats.mean_delay_pronto_soccorso1 - stats_mean.mean_delay_pronto_soccorso1)/ (i);
            stats_mean.mean_utilization_ps1 += (actual_stats.mean_utilization_ps1 - stats_mean.mean_utilization_ps1) /(i);

            stats_mean.mean_delay_pronto_soccorso2 += (actual_stats.mean_delay_pronto_soccorso2 - stats_mean.mean_delay_pronto_soccorso2)/ (i);
            stats_mean.mean_utilization_ps2 += (actual_stats.mean_utilization_ps2 - stats_mean.mean_utilization_ps2) /(i);

            double criticalValue = idfStudent(i-1,1-0.025); //alpha = 0.05
            double conf_mean_delay_118 = (criticalValue*sqrt(stats_variance.variance_delay_118 / i))/sqrt(i-1);
            double conf_mean_delay_ambulance = (criticalValue*sqrt(stats_variance.variance_delay_ambulance / i))/sqrt(i-1);
            double conf_mean_loss_118 = (criticalValue*sqrt(stats_variance.variance_loss_118 / i))/sqrt(i-1);
            double conf_mean_delay_ps1 = (criticalValue*sqrt(stats_variance.variance_delay_pronto_soccorso1/i))/sqrt(i-1);
            double conf_mean_delay_ps2 = (criticalValue*sqrt(stats_variance.variance_delay_pronto_soccorso2/i))/sqrt(i-1);
            double conf_mean_delay_a1 = (criticalValue*sqrt(stats_variance.variance_delay_accettazione1/i))/sqrt(i-1);
            double conf_mean_delay_a2 = (criticalValue*sqrt(stats_variance.variance_delay_accettazione2/i))/sqrt(i-1);

            if (i == 1 && !IMPROVEMENT_MODEL){
                fprintf(file_stats, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", i,stats_mean.mean_loss_118,0.0,
                        stats_mean.mean_delay_ambulance, 0.0, stats_mean.mean_delay_accettazione1,0.0,
                        stats_mean.mean_delay_accettazione2, 0.0,stats_mean.mean_delay_pronto_soccorso1,0.0,
                        stats_mean.mean_delay_pronto_soccorso2, 0.0);

            } else if(!IMPROVEMENT_MODEL) {
                  fprintf(file_stats, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", i,stats_mean.mean_loss_118,conf_mean_loss_118,
                        stats_mean.mean_delay_ambulance,conf_mean_delay_ambulance, stats_mean.mean_delay_accettazione1,conf_mean_delay_a1,
                        stats_mean.mean_delay_accettazione2,conf_mean_delay_a2,stats_mean.mean_delay_pronto_soccorso1,conf_mean_delay_ps1,
                        stats_mean.mean_delay_pronto_soccorso2,conf_mean_delay_ps2);
            }

            if(i == 1 && IMPROVEMENT_MODEL){
                fprintf(file_stats, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", i,stats_mean.mean_delay_118,0.0,
                    stats_mean.mean_delay_ambulance, 0.0, stats_mean.mean_delay_accettazione1,0.0,
                    stats_mean.mean_delay_accettazione2, 0.0,stats_mean.mean_delay_pronto_soccorso1,0.0,
                    stats_mean.mean_delay_pronto_soccorso2, 0.0);

            } else if (IMPROVEMENT_MODEL){
                fprintf(file_stats, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", i,stats_mean.mean_delay_118,conf_mean_delay_118,
                    stats_mean.mean_delay_ambulance,conf_mean_delay_ambulance, stats_mean.mean_delay_accettazione1,conf_mean_delay_a1,
                    stats_mean.mean_delay_accettazione2,conf_mean_delay_a2,stats_mean.mean_delay_pronto_soccorso1,conf_mean_delay_ps1,
                    stats_mean.mean_delay_pronto_soccorso2,conf_mean_delay_ps2);
            }

        }

        if(!IMPROVEMENT_MODEL) stats_variance.variance_loss_118 = stats_variance.variance_loss_118 / ITERATIONS;
        else stats_variance.variance_delay_118 = stats_variance.variance_delay_118 /ITERATIONS;
        stats_variance.variance_utilization_118 = stats_variance.variance_utilization_118 / ITERATIONS;

        stats_variance.variance_delay_ambulance = stats_variance.variance_delay_ambulance / ITERATIONS;
        stats_variance.variance_utilization_ambulance = stats_variance.variance_utilization_ambulance / ITERATIONS;

        stats_variance.variance_delay_accettazione1 = stats_variance.variance_delay_accettazione1 / ITERATIONS;
        stats_variance.variance_utilization_a1 = stats_variance.variance_utilization_a1 / ITERATIONS;

        stats_variance.variance_delay_accettazione2 = stats_variance.variance_delay_accettazione2 / ITERATIONS;
        stats_variance.variance_utilization_a2 = stats_variance.variance_utilization_a2 / ITERATIONS;

        stats_variance.variance_delay_pronto_soccorso1 = stats_variance.variance_delay_pronto_soccorso1 / ITERATIONS;
        stats_variance.variance_utilization_ps1 = stats_variance.variance_utilization_ps1 / ITERATIONS;

        stats_variance.variance_delay_pronto_soccorso2 = stats_variance.variance_delay_pronto_soccorso2 / ITERATIONS;
        stats_variance.variance_utilization_ps2 = stats_variance.variance_utilization_ps2 / ITERATIONS;

        double criticalValue = idfStudent(ITERATIONS-1,1-0.025); //alpha = 0.05

        printf("*\n* MAIN PERFORMANCE:\n");
        if(!IMPROVEMENT_MODEL) printf("* Mean loss: %lf +_ %lf \n", stats_mean.mean_loss_118, (criticalValue*sqrt(stats_variance.variance_loss_118))/sqrt(ITERATIONS-1));
        else printf("* Mean delay 118: %lf +_ %lf \n", stats_mean.mean_delay_118, (criticalValue*sqrt(stats_variance.variance_delay_118))/sqrt(ITERATIONS-1));
        printf("* Utilization system 118: %lf +_ %lf \n",stats_mean.mean_utilization_118, (criticalValue*sqrt(stats_variance.variance_utilization_118))/sqrt(ITERATIONS-1));

        printf("* Mean delay ambulance: %lf +_ %lf \n", stats_mean.mean_delay_ambulance,(criticalValue*sqrt(stats_variance.variance_delay_ambulance))/sqrt(ITERATIONS-1));
        printf("* Utilization system ambulance: %lf +_ %lf \n",stats_mean.mean_utilization_ambulance, (criticalValue*sqrt(stats_variance.variance_utilization_ambulance))/sqrt(ITERATIONS-1));

        printf("* Mean delay pronto soccorso: %lf +_ %lf \n", stats_mean.mean_delay_pronto_soccorso1,(criticalValue*sqrt(stats_variance.variance_delay_pronto_soccorso1))/sqrt(ITERATIONS-1));
        printf("* Utilization system pronto soccorso: %lf +_ %lf \n",stats_mean.mean_utilization_ps1, (criticalValue*sqrt(stats_variance.variance_utilization_ps1))/sqrt(ITERATIONS-1));

        printf("* Mean delay pronto soccorso 2: %lf +_ %lf \n", stats_mean.mean_delay_pronto_soccorso2, (criticalValue*sqrt(stats_variance.variance_delay_pronto_soccorso2))/sqrt(ITERATIONS-1));
        printf("* Utilization system pronto soccorso 2: %lf +_ %lf \n",stats_mean.mean_utilization_ps2, (criticalValue*sqrt(stats_variance.variance_utilization_ps2))/sqrt(ITERATIONS-1));

        printf("* Mean delay accettazione 1: %lf +_ %lf \n", stats_mean.mean_delay_accettazione1, (criticalValue*sqrt(stats_variance.variance_delay_accettazione1))/sqrt(ITERATIONS-1));
        printf("* Utilization system accettazione 1: %lf +_ %lf \n",stats_mean.mean_utilization_a1, (criticalValue*sqrt(stats_variance.variance_utilization_a1))/sqrt(ITERATIONS-1));

        printf("* Mean delay accettazione 2: %lf +_ %lf \n", stats_mean.mean_delay_accettazione2, (criticalValue*sqrt(stats_variance.variance_delay_accettazione2))/sqrt(ITERATIONS-1));
        printf("* Utilization system accettazione 2: %lf +_ %lf \n",stats_mean.mean_utilization_a2, (criticalValue*sqrt(stats_variance.variance_utilization_a2))/sqrt(ITERATIONS-1));
        printf("*\n\n\n");

        fclose(file_stats);
        fclose(file_stats_samples);


    }else{
        simulation_infinite_horizon();
    }
}
