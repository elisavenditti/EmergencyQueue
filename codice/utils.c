#include <stdio.h>
#include <math.h>
#include "nodi.h"
#include "rngs.h"
#include "rvms.h"



double Exponential(double m)
/* =========================================================
 * Returns an exponentially distributed positive real number.
 * NOTE: use m > 0.0
 * =========================================================
 */
{
    return (-m * log(1.0 - Random()));
}

void NextEvent(event_list_No_Priority event_118[], event_list_Priority event_Ambulance[], event_list_Priority event_ProntoSoccorso[],
                event_list_No_Priority event_Accettazione[], event_list_Priority event_ProntoSoccorso_2[],
                event_list_No_Priority event_Accettazione_2[], int res[]){
    int list_id = - 1;
    int x = - 1;
    int y = - 1;
    int z = - 1;
    int w = -1;
    int a = -1;
    int b = -1;
    int min = -1;
    int i = 0;

    // trova il primo evento attivo
    while (event_118[i].x == 0 && i < SERVERS_118 + 1)
        i++;

    if (i != SERVERS_118 + 1)  {
        x = i;
        list_id = 1;
    }

    // trova l'evento più imminente
    while (i < SERVERS_118) {
        i++;
        if ((event_118[i].x == 1) && (event_118[i].t < event_118[x].t)){
            x = i;
        }
    }

    // stampe utili
    /*for(int j=0; j<=SERVERS_118; j++) printf(" [%lf - %d] ", event_118[j].t, event_118[j].x);
    printf("118: --> prendo %d\n",x);*/

    // REINIZIALIZZO i ma salto l'entry 0 relativa agli arrivi
    // questo perchè gli arrivi all'ambulanza sono già memorizzati
    // nei completamenti del 118
    i = 1;
    while (event_Ambulance[i].x == 0 && i < SERVERS_Ambulance+1)
        i++;

    if (i != SERVERS_Ambulance + 1)  {
        y = i;
    }

    while (i < SERVERS_Ambulance) {
        i++;
        if ((event_Ambulance[i].x == 1) && (event_Ambulance[i].t < event_Ambulance[y].t)){
            y = i;
        }
    }

    /*for(int k=0; k<=SERVERS_Ambulance; k++) printf(" [%lf - %d] ", event_Ambulance[k].t, event_Ambulance[k].x);
    printf("AMBULANCE: --> prendo %d\n",y);*/

    if (x == - 1 && y == - 1) goto label;

    if ( x == -1 || ( y != -1 && event_118[x].t > event_Ambulance[y].t)){
        list_id = 2;
        min = y;
    } else {
        list_id = 1;
        min = x;
    }

label:
    i = 2;
    while (event_ProntoSoccorso[i].x == 0 && i < SERVERS_ProntoSoccorso + 2)
        i++;
    if (i != SERVERS_ProntoSoccorso + 2)  {
        z = i;
    }
    while (i < SERVERS_ProntoSoccorso + 1) {
        i++;
        if ((event_ProntoSoccorso[i].x == 1) && (event_ProntoSoccorso[i].t < event_ProntoSoccorso[z].t)){
            z = i;
        }
    }

    /*for(int k=0; k<=SERVERS_ProntoSoccorso + 1; k++) printf(" [%lf - %d] ", event_ProntoSoccorso[k].t, event_ProntoSoccorso[k].x);
    printf("PRONTO SOCCORSO: --> prendo %d\n",z);*/

    if (min == - 1 && z == -1) goto label_2;

    if (list_id == 1){
        if (min == -1 || (z!= -1 && event_118[min].t > event_ProntoSoccorso[z].t)){
            list_id = 3;
            min = z;
        }
    } else {
        if (min == -1 || (z!= -1 && event_Ambulance[min].t > event_ProntoSoccorso[z].t)){
            list_id = 3;
            min = z;
        }
    }

label_2:
    i = 0;
    while (event_Accettazione[i].x == 0 && i < SERVERS_Accettazione + 1)
        i++;
    if (i != SERVERS_Accettazione + 1)  {
        w = i;
    }
    while (i < SERVERS_Accettazione) {
        i++;
        if ((event_Accettazione[i].x == 1) && (event_Accettazione[i].t < event_Accettazione[w].t)){
            w = i;
        }
    }

    /*for(int k=0; k<=SERVERS_Accettazione; k++) printf(" [%lf - %d] ", event_Accettazione[k].t, event_Accettazione[k].x);
    printf("ACCETTAZIONE: --> prendo %d\n",w);*/

    if (min == - 1 && w == -1) goto label_3;
    if(list_id == 1){
        if (min == -1 || (w != -1 && event_118[min].t > event_Accettazione[w].t)){
            list_id = 4;
            min = w;
        }
    } else if (list_id == 2) {
        if (min == -1 || (w != -1 && event_Ambulance[min].t > event_Accettazione[w].t)){
            list_id = 4;
            min = w;
        }
    } else {
        if (min == -1 || (w != -1 && event_ProntoSoccorso[min].t > event_Accettazione[w].t)){
            list_id = 4;
            min = w;
        }
    }

label_3:
    i = 0;
    while (event_Accettazione_2[i].x == 0 && i < SERVERS_Accettazione_2 + 1)
        i++;
    if (i != SERVERS_Accettazione_2 + 1)  {
        a = i;
    }
    while (i < SERVERS_Accettazione_2) {
        i++;
        if ((event_Accettazione_2[i].x == 1) && (event_Accettazione_2[i].t < event_Accettazione_2[a].t)){
            a = i;
        }
    }

    /*for(int k=0; k<=SERVERS_Accettazione_2; k++) printf(" [%lf - %d] ", event_Accettazione_2[k].t, event_Accettazione_2[k].x);
    printf("ACCETTAZIONE_2: --> prendo %d\n",a);*/

    if (min == - 1 && a == -1) goto label_4;

    if (list_id == 1){
        if (min == -1 || (a != -1 && event_118[min].t > event_Accettazione_2[a].t)){
            list_id = 5;
            min = a;
        }
    } else if (list_id == 2) {
        if (min == -1 || (a != -1 && event_Ambulance[min].t > event_Accettazione_2[a].t)){
            list_id = 5;
            min = a;
        }
    } else if(list_id ==3){
        if (min == -1 || (a != -1 && event_ProntoSoccorso[min].t > event_Accettazione_2[a].t)){
            list_id = 5;
            min = a;
        }
    } else {
        if (min == -1 || (a != -1 && event_Accettazione[min].t > event_Accettazione_2[a].t)){
        list_id = 5;
        min = a;
        }
    }

label_4:
    i = 2;
    while (event_ProntoSoccorso_2[i].x == 0 && i < SERVERS_ProntoSoccorso_2 + 2)
        i++;
    if (i != SERVERS_ProntoSoccorso_2 + 2)  {
        b = i;
    }
    while (i < SERVERS_ProntoSoccorso_2 + 1) {
        i++;
        if ((event_ProntoSoccorso_2[i].x == 1) && (event_ProntoSoccorso_2[i].t < event_ProntoSoccorso_2[b].t)){
            b = i;
        }
    }

    /*for(int k=0; k<=SERVERS_ProntoSoccorso_2 + 1; k++) printf(" [%lf - %d] ", event_ProntoSoccorso_2[k].t, event_ProntoSoccorso_2[k].x);
    printf("PRONTO SOCCORSO_2: --> prendo %d\n",b);*/

    if (min == - 1 && b == -1) return;

    if (list_id == 1){
        if (min == -1 || (b != -1 && event_118[min].t > event_ProntoSoccorso_2[b].t)){
            list_id = 6;
            min = b;
        }
    } else if (list_id == 2) {
        if (min == -1 || (b != -1 && event_Ambulance[min].t > event_ProntoSoccorso_2[b].t)){
            list_id = 6;
            min = b;
        }
    } else if(list_id == 3){
        if (min == -1 || (b != -1 && event_ProntoSoccorso[min].t > event_ProntoSoccorso_2[b].t)){
            list_id = 6;
            min = b;
        }
    } else if (list_id == 4) {
        if (min == -1 || (b != -1 && event_Accettazione[min].t > event_ProntoSoccorso_2[b].t)){
            list_id = 6;
            min = b;
        }
    }
    else {
        if (min == -1 || (b != -1 && event_Accettazione_2[min].t > event_ProntoSoccorso_2[b].t)){
            list_id = 6;
            min = b;
        }
    }

    res[0] = min;
    res[1] = list_id;

    //printf ("IL MINIMO: %d nella lista %d", min, list_id);

}


int generateProntoSoccorso(statoProntoSoccorso stato1, statoProntoSoccorso stato2){
    //1 ProntoSoccorso_1
    //0 ProntoSoccorso_2

    if(IMPROVEMENT_MODEL){
        if(stato1.numberQueue <= stato2.numberQueue) return (1);
        return (0);
    }
    else {
        SelectStream(16);
        if (Random() < 0.5) return (1);
        return (0);
    }
}
