#if !defined(NODI_H_INCLUDED)
#define NODI_H_INCLUDED

#define SERVERS_118  2
#define SERVERS_Ambulance  5
#define SERVERS_ProntoSoccorso  5
#define SERVERS_ProntoSoccorso_2  5
#define SERVERS_Accettazione  2
#define SERVERS_Accettazione_2  2

#define IMPROVEMENT_MODEL 0
#define FINITE_HORIZON 0

#define N_CENTRI 6

// nella simulazone a orizzonte finito decommentare:
//#define STOP        720.0
// nella simulazone a orizzonte infinito decommentare:
#define STOP        200000.0
#define START       0.0
#define INFINITY   (100.0 * STOP)
#define BURST_TIME 30.0
#define SAMPLE_TIME 20.0

int restart[3];
double queueCritical[5000000];
double queueNotCritical[5000000];

typedef struct{
      long numberNode;
      long number_critical_Node;

      long numberQueue;
      long number_critical_Queue;

      long numberCompletion;

} statoAmbulance;

typedef struct{
      long numberNode;
      long number_Red_Node;
      long number_Yellow_Node;

      long numberQueue;
      long number_Red_Queue;
      long number_Yellow_Queue;


      long numberCompletion;
      long number_Red_Completion;
      long number_Yellow_Completion;

} statoProntoSoccorso;


typedef struct{
      long numberNode;
      long numberCompletion;
      long numberQueue;
} stato118;

typedef struct{
      long numberNode;
      long numberCompletion;
      long numberQueue;
} statoAccettazione;

typedef struct {                        /* the next-event list    */
  double t;                             /*   next event time      */
  int    x;                             /*   event status, 0 or 1 */
} event_list_No_Priority;

typedef struct {                        /* the next-event list    */
  double t;                             /*   next event time      */
  int    x;
  int type;                         /*   event status, 0 or 1 */
} event_list_Priority;

typedef struct {                           /* accumulated sums of                */
    double service;                  /*   service times                    */
    long   served;                   /*   number served                    */
} sum_server_No_Priority;

typedef struct {                           /* accumulated sums of                */
    double service;                  /*   service times                    */
    long  servedCritical;                   /*   number served                    */
    long served;
} sum_server_Priority;

typedef struct {
    double node;
    double queue;
    double service;
} area;

typedef struct{
    double mean_loss_118;
    double mean_delay_118;
    double mean_utilization_118;

    double mean_delay_pronto_soccorso1;
    double mean_utilization_ps1;

    double mean_delay_pronto_soccorso2;
    double mean_utilization_ps2;

    double mean_delay_accettazione1;
    double mean_utilization_a1;

    double mean_delay_accettazione2;
    double mean_utilization_a2;

    double mean_delay_ambulance;
    double mean_utilization_ambulance;
} statistics_mean;

typedef struct{
    double variance_loss_118;
    double variance_delay_118;
    double variance_utilization_118;

    double variance_delay_pronto_soccorso1;
    double variance_utilization_ps1;

    double variance_delay_pronto_soccorso2;
    double variance_utilization_ps2;

    double variance_delay_accettazione1;
    double variance_utilization_a1;

    double variance_delay_accettazione2;
    double variance_utilization_a2;

    double variance_delay_ambulance;
    double variance_utilization_ambulance;
} statistics_variance;

double Exponential(double m);
void NextEvent(event_list_No_Priority event_118[], event_list_Priority event_Ambulance[], event_list_Priority event_ProntoSoccorso[],
                event_list_No_Priority event_Accettazione[], event_list_Priority event_ProntoSoccorso_2[],
                event_list_No_Priority event_Accettazione_2[], int res[]);
void simulation_finite_horizon(int i,FILE* file,statistics_mean* stats);
void simulation_infinite_horizon(void);
int generateProntoSoccorso(statoProntoSoccorso stato1, statoProntoSoccorso stato2);


void SetupNode_118 (event_list_No_Priority* event, sum_server_No_Priority* sum);
double GetArrival_118(void);
double GetService_118(void);
int FindOne118(event_list_No_Priority event[]);
void arrivalAt118 (double current, event_list_No_Priority event[],sum_server_No_Priority sum[], stato118* stato118, long* reject);
void completionAt118 (double current, event_list_No_Priority event_118[], event_list_Priority event_Ambulance[], stato118* stato118, statoAmbulance* statoAmbulanza,
                      sum_server_No_Priority sum_118[],sum_server_Priority sum_Ambulance[], double* stop118,int e);

void SetupNode_Ambulance (event_list_Priority* event, sum_server_Priority* sum);
double GetService_Ambulance();
int FindOneAmbulance(event_list_Priority event[]);
int GenerateType (void);
void arrivalAtAmbulance(double current, event_list_Priority event[], sum_server_Priority sum[], statoAmbulance* stato);
void completionAtAmbulance (double current, event_list_Priority event_Ambulance[],sum_server_Priority sum_Ambulance[],  statoAmbulance* statoAmbulanza, stato118* stato118,
                              int e);
int generateDeath();
int GenerateType_From_Ambulance(void);

void SetupNode_ProntoSoccorso (event_list_Priority* event, sum_server_Priority* sum);
double GetService_ProntoSoccorso();
int FindOneProntoSoccorso(event_list_Priority event[]);
void arrivalAtProntoSoccorso(double current, int type, event_list_Priority event[], sum_server_Priority sum[], statoProntoSoccorso* stato, long* deathPS);
void completionAtProntoSoccorso (double current, event_list_Priority event[], sum_server_Priority sum[], statoProntoSoccorso* stato, statoAmbulance* statoAmbulance, double* stopPS, int e);

int GenerateType_FromAccettazione(void);
void SetupNode_Accettazione (event_list_No_Priority* event,sum_server_No_Priority* sum);
double GetArrival_Accettazione();
double GetService_Accettazione();
int FindOneAccettazione(event_list_No_Priority event[]);
void arrivalAtAccettazione(double current, event_list_No_Priority event_Accettazione[], sum_server_No_Priority sum_Accettazione[],
                           statoAccettazione* statoAccettazione);
void completionAtAccettazione (double current, event_list_No_Priority event_Accettazione[], event_list_Priority event_ProntoSoccorso[],  statoAccettazione* statoAccettazione, statoProntoSoccorso* statoProntoSoccorso, sum_server_No_Priority sum_Accettazione[],
                                sum_server_Priority sum_ProntoSoccorso [], double* stopAccettazione,  int e, long* deathPS);


void SetupNode_ProntoSoccorso_2 (event_list_Priority* event, sum_server_Priority* sum);
double GetService_ProntoSoccorso_2();
int FindOneProntoSoccorso_2(event_list_Priority event[]);
void arrivalAtProntoSoccorso_2(double current, int type, event_list_Priority event[], sum_server_Priority sum[], statoProntoSoccorso* stato, long* deathPS);
void completionAtProntoSoccorso_2 (double current, event_list_Priority event[], sum_server_Priority sum[], statoProntoSoccorso* stato, statoAmbulance* statoAmbulance, double* stopPS_2,int e);


int GenerateType_FromAccettazione_2(void);
double GetArrival_Accettazione_2();
double GetService_Accettazione_2();
void SetupNode_Accettazione_2 (event_list_No_Priority* event,sum_server_No_Priority* sum);
int FindOneAccettazione_2(event_list_No_Priority event[]);
void arrivalAtAccettazione_2(double current, event_list_No_Priority event_Accettazione[], sum_server_No_Priority sum_Accettazione[],
                           statoAccettazione* statoAccettazione);
void completionAtAccettazione_2 (double current, event_list_No_Priority event_Accettazione[], event_list_Priority event_ProntoSoccorso[],  statoAccettazione* statoAccettazione, statoProntoSoccorso* statoProntoSoccorso, sum_server_No_Priority sum_Accettazione[],
                                sum_server_Priority sum_ProntoSoccorso [], double* stopAccettazione,  int e, long* deathPS);


#endif // NODI_H_INCLUDED
