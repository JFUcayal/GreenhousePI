#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <mqueue.h>
#include <ctime>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>
#include <csignal>
#include <thread>


#include "sensors.h"
#include "actuators.h"
#include "Database.h"

//struct que guarda valores lidos dos sensores
struct sensors_data {

    string timestamp;
    float temperature;
    float air_humidity;
    float soil_humidity;
    float water_level;
    float light_level;    
};

//struct que guarda pretendidos para estufa
struct define_data{
    float define_temperature;
    float define_humidity;
    float define_soilhumidity;
    bool define_light;
};

//struct que guarda estados dos atuadores
struct actuators_state{
    
    bool heater_state;
    bool fan_state;
    bool water_sprinklers_state;
    bool watering_state;
    bool light_state;
    bool door_state;
};

class System{
    private:
        define_data define_data_inst;                   //guarda defines atuais
        actuators_state realtime_actuators_state;       //guarda estado atual dos atuadores
        //sensores do sistema
        Temp_Hum_Sensor temp_hum_sensor;
        LDR_Sensor light_sensor;
        Soil_Hum_Sensor soil_h_sensor; 
        Water_Level_Sensor water_sensor;
        //atuadores do sistema
        Actuators relay_module;
        //apontador para database
       static Database* Greenhouse_Database;
        //mutexes utilizados no sistema
        pthread_mutex_t mutex_ReadSensors_msg;
        pthread_mutex_t mutex_Database;
        pthread_mutex_t mutex_Actuators;
        //message queues utilizadas
        mqd_t QueueDatabase;
        mqd_t myQueue;
        //static mqd_t QueueActuators;
        static mqd_t QueueActuators;
        static mqd_t QueuesendRemote;
        mqd_t QueuereceiveRemote;

        pthread_t readSensorsID;
        pthread_t CalculateRefID;
        pthread_t databaseID;
        pthread_t actuatorsID;
        pthread_t sendRemoteDataID;
        pthread_attr_t attr;
        pthread_attr_t attr_2;
        pthread_attr_t attr_3;
        pthread_attr_t attr_4;
        pthread_attr_t attr_5;
        pthread_attr_t attr_6;
        struct sched_param param;
        struct sched_param param2;
        struct sched_param param3;
        struct sched_param param4;
        struct sched_param param5;
        struct sched_param param6;


        static volatile sig_atomic_t receivedSignal;
        static pthread_mutex_t mutex_Door;

        //funções utilizadas para envio e receção de dados das message queues, ambas utilizam o mesmo "template"
        int sendToQueue(mqd_t queue, const sensors_data& data) {
            if (mq_send(queue, reinterpret_cast<const char*>(&data), sizeof(sensors_data), 0) == -1) {
                perror("mq_send");
                return 1;
            }
            return 0;
        }

        int receiveFromQueue(mqd_t queue, sensors_data& data) {
            if (mq_receive(queue, reinterpret_cast<char*>(&data), sizeof(sensors_data), nullptr) == -1) {
                perror("mq_receive");
                return 1;
            }
            return 0;
        }

    public:
        System(const char* dbName); //construtor
        ~System();                  //destrutor
        void startUp();
        time_t get_time();          //devolve tempo
        sensors_data get_sensor_data();     //lê sensores
        //funções para definir e ler defines
        define_data get_define_data();
        void set_define_data(const define_data&);
        //funções relacionadas aos atuadores
        actuators_state get_actuators_state();
        void initial_actuators_state_def();
          
        //funções relacionadas às message queues
        int sendToDatabase(const sensors_data&);
        int receiveFromDatabase(sensors_data&);  

        int sendTomyQueue(const sensors_data&);
        int receiveFrommyQueue(sensors_data&); 
        
       // int sendToActuators(const int*);
       // int receiveFromActuators(int*);  

        //funções relacionadas à database
        void getDefineValuesDatabase(define_data&);
        void getDataValuesDatabase(sensors_data&);
        void aux_SIGTERM();

        static void signalHandler(int signum);
        static void alarmHandler(int signum);
        static void sendValuesHandler(int signum);

        static void* tReadSensors(void* arg);
        static void* tCalculateRef(void* arg);
        static void* tDatabase(void* arg);
        static void* tActuators(void* arg);
        static void* tsendRemote(void* arg);

        bool initializeThreads();
        void joinThreads();



};  


#endif
