#include "system.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sqlite3.h> 
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <mqueue.h>
#include <cstring>
#include <vector>
#include <cmath> 
#include <ctime>
#include <cstdlib>
#include <signal.h>
#include <fcntl.h>
#include "logger.h"
#include <vector>
#include <sstream>

using namespace std;

#define TEMP_MARGIN 2
#define HUM_MARGIN 1
#define HUMSOIL_MARGIN 1
#define CRITIC_WATERLEVEL 5
#define LIGHT_MARGIN 1
#define LIGHT_DEFINE 50

#define DEVICE_PATH "/dev/gpio_irq"
#define REGIST_PID 0x100 // Valor fictício, ajuste conforme necessário

//pthread_mutex_t System::mutex_Door = PTHREAD_MUTEX_INITIALIZER;
//mqd_t System::QueueActuators = 0;

//variáveis staticS
volatile sig_atomic_t System::receivedSignal = 0;
pthread_mutex_t System::mutex_Door = PTHREAD_MUTEX_INITIALIZER;
mqd_t System::QueueActuators = 0;
mqd_t System::QueuesendRemote = 0;
int envio = 0;
Database* System::Greenhouse_Database = nullptr;

//construtor
System::System(const char* dbName) {

    system("modprobe i2c-dev");
    system("modprobe i2c-bcm2835");

    const char *comando_gpio_irq = "insmod gpio_irq.ko";
    int resultado_gpio_irq = std::system(comando_gpio_irq);

    // Verificar se a execução foi bem-sucedida
    if (resultado_gpio_irq == 0) {
        // O comando foi executado com sucesso
        std::cout << "Módulo do driver carregado com sucesso.\n";
    } else {
        //comando falhou
        //std::cerr << "Falha ao carregar o módulo do driver.\n";
        aviso("Falha ao carregar o módulo do driver");
    }

    // Obtem o tempo
    time_t timestamp = get_time();
    tm *tm_info = localtime(&timestamp);

    usleep(100);
    char sampleTimeStr[20]; // Tamanho suficiente para armazenar a data e hora formatadas
    strftime(sampleTimeStr, sizeof(sampleTimeStr), "%d-%m-%Y %H:%M:%S", tm_info);   //converte a variável time numa string

    cout << "****************************************" << endl;
    cout << "System Created: " << sampleTimeStr << endl ;

    Greenhouse_Database = new Database(dbName);             //inicia database

    //abre decive driver para GPIO17 com interrupt
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        erro("Falha ao abrir o dispositivo");
        delete Greenhouse_Database;
        close(fd);
    }

    // Configurar o PID usando a IOCTL
    if (ioctl(fd, REGIST_PID, 0) == -1) {
        erro("Falha ao configurar PID usando IOCTL");
        delete Greenhouse_Database;
        close(fd);
    //    return -1;
    }
    close(fd);

    startUp();

    Greenhouse_Database->insertDefineValues(20.00, 25.99, 95, true);
    
    pthread_mutex_lock(&mutex_Database);
    //definir valores predifinidos vindos da database
    define_data storageData;
    getDefineValuesDatabase(define_data_inst);
    pthread_mutex_unlock(&mutex_Database);

    initial_actuators_state_def();
    signal(SIGUSR1, &System::signalHandler);
    signal(SIGALRM, &System::alarmHandler);
  //  signal(SIGINT, &System::signalHandler);
  
    signal(SIGTERM, &System::signalHandler);

    // Cria uma estrutura de temporizador
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;

    // Configuração da estrutura sigevent para notificar via sinal
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGTERM;

    // Cria um temporizador
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    // Configuração do temporizador para disparar a cada 15 segundos
    its.it_value.tv_sec = 15;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 10;
    its.it_interval.tv_nsec = 0;

    // Configura o temporizador
    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }


    initializeThreads();
    joinThreads();
}

/// @brief start-up sequence
void System::startUp(){

    // Inicializar o mutex
    mutex_ReadSensors_msg = PTHREAD_MUTEX_INITIALIZER;
    //se dá erro ao gerar o mutex liberta o espaço armazenado para os recursos anteriores
    if (pthread_mutex_init(&mutex_ReadSensors_msg, NULL) != 0) {
        std::cerr << "Mutex initialization failed" << std::endl;
        delete Greenhouse_Database;
    }

    mutex_Database = PTHREAD_MUTEX_INITIALIZER;
    //se dá erro ao gerar o mutex liberta o espaço armazenado para os recursos anteriores
    if (pthread_mutex_init(&mutex_Database, NULL) != 0) {
        std::cerr << "Mutex initialization failed" << std::endl;
        delete Greenhouse_Database;
        pthread_mutex_destroy(&mutex_ReadSensors_msg);
    }


    mutex_Actuators = PTHREAD_MUTEX_INITIALIZER;
    //se dá erro ao gerar o mutex liberta o espaço armazenado para os recursos anteriores
    if (pthread_mutex_init(&mutex_Actuators, NULL) != 0) {
        std::cerr << "Mutex initialization failed" << std::endl;
        delete Greenhouse_Database;
        pthread_mutex_destroy(&mutex_ReadSensors_msg);
        pthread_mutex_destroy(&mutex_Database);
    }

    //cria message queue atuadores
    mq_unlink("/actuators");
    struct mq_attr attr_3;
    attr_3.mq_maxmsg = 2;
    attr_3.mq_msgsize = sizeof(int) * 6;
    
    const char* msgqueue_Actuators = "/actuators";
    QueueActuators = mq_open(msgqueue_Actuators, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr_3);

    //se dá erro ao gerar a messsage queue liberta o espaço armazenado para os recursos anteriores
    if (QueueActuators == (mqd_t)-1){
        std::cerr << "Error creating message queues" << std::endl;
        perror("mq_open");    
        delete Greenhouse_Database;
        pthread_mutex_destroy(&mutex_ReadSensors_msg);
        pthread_mutex_destroy(&mutex_Database);
        pthread_mutex_destroy(&mutex_Actuators);
    }


    //cria message queue database
    mq_unlink("/Database");
    struct mq_attr attr_2;
    attr_2.mq_maxmsg = 1;
    attr_2.mq_msgsize = sizeof(sensors_data);

    const char* msgqueue_Database = "/Database";
    QueueDatabase = mq_open(msgqueue_Database, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr_2);

    //se dá erro ao gerar a messsage queue liberta o espaço armazenado para os recursos anteriores
    if (QueueDatabase == (mqd_t)-1){
        std::cerr << "Error creating message queues" << std::endl;
        perror("mq_open");    
        delete Greenhouse_Database;
        pthread_mutex_destroy(&mutex_ReadSensors_msg);
        pthread_mutex_destroy(&mutex_Database);
        pthread_mutex_destroy(&mutex_Actuators);
        mq_close(QueueActuators);
    }


    //cria message queue Myqueue para sensores
    mq_unlink("/read_sensors");
    struct mq_attr attr;
    attr.mq_maxmsg = 1; 
    attr.mq_msgsize = sizeof(sensors_data);  // Tamanho de uma mensagem (float)

    const char* msgqueue_ReadSensors = "/read_sensors";
    myQueue = mq_open(msgqueue_ReadSensors, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);
    
    //se dá erro ao gerar a messsage queue liberta o espaço armazenado para os recursos anteriores
    if (myQueue == (mqd_t)-1) {
        std::cerr << "Error creating message queues" << std::endl;
        perror("mq_open");    
        delete Greenhouse_Database;
        pthread_mutex_destroy(&mutex_ReadSensors_msg);
        pthread_mutex_destroy(&mutex_Database);
        pthread_mutex_destroy(&mutex_Actuators);
        mq_close(QueueActuators);
        mq_close(QueueDatabase);
    } 
    

    //cria thread para escrever para sistema remoto
    mq_unlink("/sendRemote");
    struct mq_attr attr_4;
    attr_4.mq_maxmsg = 2; 
    attr_4.mq_msgsize = sizeof(float)* 11;
    //QueuesendRemote

    const char* msgqueue_sendRemote = "/sendRemote";
    // Abra a fila de mensagens sem redefinir os atributos
    QueuesendRemote = mq_open(msgqueue_sendRemote, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR, &attr_4);
    
    //se dá erro ao gerar a messsage queue liberta o espaço armazenado para os recursos anteriores
    if (QueuesendRemote == (mqd_t)-1) {
        std::cerr << "Error creating message queues" << std::endl;
        perror("mq_open");    
        delete Greenhouse_Database;
        pthread_mutex_destroy(&mutex_ReadSensors_msg);
        pthread_mutex_destroy(&mutex_Database);
        pthread_mutex_destroy(&mutex_Actuators);
        mq_close(QueueActuators);
        mq_close(QueueDatabase);
        mq_close(myQueue);
    }        
    else {
        cout << "Message queues created successfully!" << std::endl;
    }


    //cria thread para receber dados sistema remoto
    mq_unlink("/receiveRemote");
    struct mq_attr attr_5;
    attr_5.mq_maxmsg = 2; 
    attr_5.mq_msgsize = sizeof(float) *4;
    //QueuereceiveRemote    QueuereceiveRemote

    const char* msgqueue_ReceiveRemote = "/receiveRemote";  
    // Abra a fila de mensagens sem redefinir os atributos
    QueuereceiveRemote = mq_open(msgqueue_ReceiveRemote, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR, &attr_5);
    
    //se dá erro ao gerar a messsage queue liberta o espaço armazenado para os recursos anteriores
    if (QueuereceiveRemote == (mqd_t)-1) {
        std::cerr << "Error creating message queues" << std::endl;
        perror("mq_open");    
        delete Greenhouse_Database;
        pthread_mutex_destroy(&mutex_ReadSensors_msg);
        pthread_mutex_destroy(&mutex_Database);
        pthread_mutex_destroy(&mutex_Actuators);
        mq_close(QueueActuators);
        mq_close(QueueDatabase);
        mq_close(myQueue);
        mq_close(QueuesendRemote);
    }        
    else {
        cout << "Message queues created successfully!" << std::endl;
    }

}


//destrutor
System::~System(){
    cout << "****************************************" << endl;
    cout << "System Destroyed" << endl ;

    const char *comando_gpio_irq = "rmmod gpio_irq.ko";
    int resultado_gpio_irq = std::system(comando_gpio_irq);

    // Verificar se a execução foi bem-sucedida
    if (resultado_gpio_irq == 0) {
        // O comando foi executado com sucesso
        std::cout << "Módulo do driver retirado com sucesso.\n";
    } else {
        //comando falhou
        std::cerr << "Falha ao retirar módulo do driver.\n";
    }

    mq_close(QueueDatabase);
    mq_close(myQueue);
    mq_close(QueueActuators);
    mq_close(QueuesendRemote);
    mq_close(QueuereceiveRemote);

    pthread_mutex_destroy(&mutex_ReadSensors_msg);
    pthread_mutex_destroy(&mutex_Database);
    pthread_mutex_destroy(&mutex_Actuators);

    delete Greenhouse_Database;
}

//retorna data e hora atual
time_t System::get_time(){

    time_t curr_time;
    time(&curr_time);

    return curr_time;

}

/*
//define estado dos atuaores
void System::set_actuators_state(string actuator_name, bool set_state){

    //Actuators atuadores;
    relay_module.set_Actuators_State(actuator_name, set_state);

    if (actuator_name == "FAN") {
        realtime_actuators_state.fan_state = set_state;
    } else if (actuator_name == "HEATER") {
        realtime_actuators_state.heater_state = set_state;
    } else if (actuator_name == "WATER_SPRINKLER") {
        realtime_actuators_state.water_sprinklers_state = set_state;
    } else if (actuator_name == "WATERING") {
        realtime_actuators_state.watering_state = set_state;
    } else if (actuator_name == "LIGHT") {
        realtime_actuators_state.light_state = set_state;
    } else if (actuator_name == "DOOR") {
        realtime_actuators_state.door_state = set_state;
    }

}
*/

//FUNÇÕES STRUCTS
sensors_data System::get_sensor_data() {

    sensors_data system_data;

    // Obtenha o timestamp
    time_t timestamp = get_time();
    tm *tm_info = localtime(&timestamp);

    char sampleTimeStr[20]; // Tamanho suficiente para armazenar a data e hora formatadas
    strftime(sampleTimeStr, sizeof(sampleTimeStr), "%d-%m-%Y %H:%M:%S", tm_info);   //converte a variável time numa string

    system_data.timestamp =sampleTimeStr;

    system_data.temperature = temp_hum_sensor.get_Temperature();
    system_data.air_humidity = temp_hum_sensor.get_Humidity();
    system_data.soil_humidity = soil_h_sensor.get_soil_moisture();
    system_data.water_level = water_sensor.get_water_level();
    system_data.light_level = light_sensor.get_brightness_value();

    cout << " Temp  -> " << system_data.temperature << endl
         << " A_Hum -> " << system_data.air_humidity << endl
         << " S_Hum -> " << system_data.soil_humidity << endl
         << " Water -> " << system_data.water_level << endl
         << " Light -> " << system_data.light_level << endl;

    return system_data;
}

define_data System::get_define_data(){ 
    return define_data_inst; 
}

void System::set_define_data(const define_data& define_data_func){ 
    define_data_inst = define_data_func; 
}

actuators_state System::get_actuators_state(){ 
    return realtime_actuators_state; 
}

//FUNÇÕES MESSAGE QUEUES
int System::sendTomyQueue(const sensors_data& data){
    return sendToQueue(myQueue, data);
}

int System::receiveFrommyQueue(sensors_data& data){
    return receiveFromQueue(myQueue, data);
} 

int System::sendToDatabase(const sensors_data& data) {
    return sendToQueue(QueueDatabase, data);
}

int System::receiveFromDatabase(sensors_data& data) {
    return receiveFromQueue(QueueDatabase, data);
}  

/*int System::sendToActuators(const int* buffer){

    if (mq_send(QueueActuators, reinterpret_cast<const char*>(buffer), sizeof(int) * 6, 0) == -1) {
            perror("mq_send");
            return 1;  // Retorne um valor indicando falha
        }

        return 0;  // Retorne um valor indicando sucesso
}

int System::receiveFromActuators(int* buffer){

    if (mq_receive(QueueActuators, reinterpret_cast<char*>(buffer), sizeof(int) * 6, 0) == -1) {
            perror("mq_receive");
            return 1;  // Retorne um valor indicando falha
        }

        return 0;  // Retorne um valor indicando sucesso
}
*/


void System::initial_actuators_state_def(){

    relay_module.initialize_Actuators();

    realtime_actuators_state.fan_state = false;
    realtime_actuators_state.heater_state = false;
    realtime_actuators_state.light_state = false;
    realtime_actuators_state.water_sprinklers_state = false;
    realtime_actuators_state.watering_state = false;
    realtime_actuators_state.door_state = false;
}


void System::getDefineValuesDatabase(define_data& data){

    Greenhouse_Database->getDefineValues(data.define_temperature, data.define_humidity, data.define_soilhumidity, data.define_light);
}


void System::getDataValuesDatabase(sensors_data& data){
    Greenhouse_Database->getDataValues(data.timestamp, data.temperature, data.air_humidity, data.soil_humidity, data.water_level, data.light_level);
}


//FUNÇÕES SIGNALS
void System::signalHandler(int signum) {

    if(signum == SIGUSR1){
        pthread_mutex_lock(&mutex_Door);
        receivedSignal = 1;
        int vetorDoor [6] = {0, 0, 0, 0, 0, 1};
        if (mq_send(QueueActuators, reinterpret_cast<const char*>(vetorDoor), sizeof(int) * 6, 0) == -1) {
            perror("mq_send");
        }

        // Adicione lógica adicional aqui, se necessário
        std::cout << "Recebeu o sinal SIGUSR1" << std::endl;

        alarm(5);
        pthread_mutex_unlock(&mutex_Door);
    }
   
    else if(signum == SIGTERM){

        float sendBuffer[11];
        float databaseData[5];
        std::cout << "Recebeu o sinal SIGTERM" << std::endl;

        string time;

        Greenhouse_Database->getDataValues(time, databaseData[0], databaseData[1], databaseData[2], databaseData[3], databaseData[4]);
        /*
        istringstream ss(time);
        string token;
        

        // Itera sobre cada parte separada por espaços e hífens
        while (std::getline(ss, token, ' ') || std::getline(ss, token, '-') || std::getline(ss, token, ':')) {
            result.push_back(std::stof(token));
        }
        */
        vector<float> result;

        std::tm tm = {};
        std::istringstream ss(time);
        char delimiter;

        ss >> tm.tm_mday >> delimiter >> tm.tm_mon >> delimiter >> tm.tm_year >> tm.tm_hour >> delimiter >> tm.tm_min >> delimiter >> tm.tm_sec;

        if (ss.fail()) {
            std::cerr << "Erro: Formato de timestamp inválido." << std::endl;
        }

        result.push_back(static_cast<float>(tm.tm_mday));  // Dia
        result.push_back(static_cast<float>(tm.tm_mon ));  // Mês (adicionando 1 porque tm_mon é base 0)
        result.push_back(static_cast<float>(tm.tm_year ));  // Ano (adicionando 1900 porque tm_year é anos desde 1900)
        result.push_back(static_cast<float>(tm.tm_hour));  // Hora
        result.push_back(static_cast<float>(tm.tm_min));  // Minuto
        result.push_back(static_cast<float>(tm.tm_sec));  // Segundo

        //Time
        sendBuffer[0] = result[0];
        sendBuffer[1] = result[1];
        sendBuffer[2] = result[2];
        sendBuffer[3] = result[3];
        sendBuffer[4] = result[4];
        sendBuffer[5] = result[5];
        
        sendBuffer[6] = databaseData[0];
        sendBuffer[7] = databaseData[1];
        sendBuffer[8] = databaseData[2];
        sendBuffer[9] = databaseData[3];
        sendBuffer[10] = databaseData[4];


        cout << "T " << result[0] << endl;
        cout << "T1 " << result[1]<< endl;
        cout << "T2 " << result[2]<< endl;
        cout << "T3 " << result[3]<< endl;
        cout << "T4 " << result[4]<< endl;
        cout << "T5 " << result[5]<< endl;
        
        cout << "T " << databaseData[0]<< endl;
        cout << "A " << databaseData[1]<< endl;
        cout << "S " << databaseData[2]<< endl;
        cout << "W " << databaseData[3]<< endl;
        cout << "L " << databaseData[4]<< endl;

        ssize_t result_data = mq_send(QueuesendRemote, reinterpret_cast<const char*>(sendBuffer), sizeof(float) * 11, 0);

        if (result_data == -1) {
            std::cerr << "Error sending data from Process 1" << std::endl;
            perror("mq_send");
        } else {
            std::cout << "Data sent from Process 1" << std::endl;
        }  
    }
}


void System::alarmHandler(int signum) {

    if(signum == SIGALRM){
        pthread_mutex_lock(&mutex_Door);
        receivedSignal = 2;
        int vetorDoor [6] = {0, 0, 0, 0, 0, 2};
        if (mq_send(QueueActuators, reinterpret_cast<const char*>(vetorDoor), sizeof(int) * 6, 0) == -1) {
            perror("mq_send");
        }

        // Adicione lógica adicional aqui, se necessário
        std::cout << "Recebeu o sinal SIGALARM" << std::endl;

        alarm(0);
        pthread_mutex_unlock(&mutex_Door);
    }
}


void System::sendValuesHandler(int signum){
}


/*void System::signalHandler(int signum){
//envio para messsage queue dos atuadores o "1" da porta -> abrir porta

    pthread_mutex_lock(&mutex_Door);
    //receivedSignal = 1;
    int openDoor2[6] = {0, 0, 0, 0, 0, 1};
    pthread_mutex_lock(&mutex_Door);

    cout << "entrou no signal handler de abrir a porta!" << endl;
    if (mq_send(QueueActuators, reinterpret_cast<const char*>(openDoor2), sizeof(int) * 6, 0) == -1) {
        perror("mq_send");
    }
    
 //   relay_module.set_Actuators_State("DOOR", true);
    alarm(5);
    pthread_mutex_unlock(&mutex_Door);

}

void System::alarmHandler(int signum){
    //envio para message queue dos atuadores "0" da porta -> fechar porta
    int openDoor2[6] = {0, 0, 0, 0, 0, 2};
    pthread_mutex_lock(&mutex_Door);

    if (mq_send(QueueActuators, reinterpret_cast<const char*>(openDoor2), sizeof(int) * 6, 0) == -1) {
        perror("mq_send");
    }
 //   local_sys.set_actuators_state("DOOR", false);
    pthread_mutex_unlock(&mutex_Door);
    // Cancelar o alarme
    alarm(0);
}
*/


//funções configuração threads
void SetupThread(int prio, pthread_attr_t *pthread_attr, struct sched_param *pthread_param){
    
    using namespace std;
    int rr_min_priority, rr_max_priority;

    //retornam os valores minimo e maximo de prioridade disponivel para o escalonamento
    rr_min_priority = sched_get_priority_min (SCHED_RR);
    rr_max_priority = sched_get_priority_max (SCHED_RR);

    pthread_attr_init(pthread_attr); //inicializa atributos da thread com valores default
    
    pthread_attr_setinheritsched (pthread_attr, PTHREAD_EXPLICIT_SCHED);  //configura atributos da thread para que a política de escalonamento seja 
    //determinada pelos atributos, e não herde a política do processo principal

    pthread_attr_setschedpolicy (pthread_attr, SCHED_RR);  //define processo de escalonamento como o Roud Robin (RR)

    //define o valor da prioridade da struct
    pthread_param -> sched_priority = prio;
    std::cout << "SCHED_RR priority range is " << rr_min_priority << " to " 
         << rr_max_priority << ": using " << pthread_param -> sched_priority << endl;

    //parâmetros de escalonamento são associados à thread (prioridade e tipo de esclonamento (RR) por exemplo)
    pthread_attr_setschedparam (pthread_attr, pthread_param);
}

bool System::initializeThreads() {
    SetupThread(1, &attr, &param);
    SetupThread(2, &attr_2, &param2);
    SetupThread(3, &attr_3, &param3);
    SetupThread(1, &attr_4, &param4);
    SetupThread(4, &attr_5, &param5);

    int rSensors = pthread_create(&readSensorsID, &attr, &System::tReadSensors, this);
    int rCalculate = pthread_create(&CalculateRefID, &attr_2, &System::tCalculateRef, this);
    int rDatabase = pthread_create(&databaseID, &attr_3, &System::tDatabase, this);
    int rActuators = pthread_create(&actuatorsID, &attr_4, &System::tActuators, this);
    int rSendRemote = pthread_create(&sendRemoteDataID, &attr_5, &System::tsendRemote, this);;

    if (rSensors != 0 || rCalculate != 0 || rDatabase != 0 || rActuators != 0 || rSendRemote != 0) {
        erro("Error creating threads");
        return 1;
    }
    else{
        return 0;
    }
}

void System::joinThreads() {
    pthread_join(readSensorsID, nullptr);
    pthread_join(CalculateRefID, nullptr);
    pthread_join(databaseID, nullptr);
    pthread_join(actuatorsID, nullptr);
    pthread_join(sendRemoteDataID, nullptr);
}

//funções das thread
void *System::tReadSensors(void *arg){ 

    sensors_data system_data;
    System *local_sys = static_cast<System*>(arg);

    while(1) {
        std::cout << "entrou thread sensores" << endl;

        pthread_mutex_lock(&local_sys->mutex_ReadSensors_msg);
        
        system_data = local_sys->get_sensor_data();

        std::cout << "horas: " << system_data.timestamp << endl;

        pthread_mutex_unlock(&local_sys->mutex_ReadSensors_msg);

       const char *ptr = reinterpret_cast<const char *>(&system_data);

       int result = local_sys->sendTomyQueue(system_data);
       std::cout << "resultado do envio: " << result << endl << flush;

       if (result == -1) {
        perror("mq_send");
       }
       else {
        std::cout << "Message sent successfully!" << endl << flush; ;
       }
        sleep(1);  // Aguarda por 100ms
    }
}

float calculateAverage(const vector<float>& buffer) {
   
    if (buffer.empty()) {
        return 0.0;  // Retorna 0 se o vetor estiver vazio para evitar divisão por zero
    }

    float sum = 0.0;
    for (size_t i = 0; i < buffer.size(); ++i) {
        sum += buffer[i];
    }
    return (sum / buffer.size());
}

void *System::tCalculateRef(void *arg){ 

    int buffer_calculate_values [sizeof(int) * 6];
    vector<float> buffer_temperature;
    vector<float> buffer_humidity;
    vector<float> buffer_soilhumidity;
    vector<float> buffer_waterlevel;
    vector<float> buffer_light;
    sensors_data received_data;
    sensors_data send_average_data;
    char buffer[sizeof(sensors_data)];  // Buffer fixo para a mensagem
    System *local_sys = static_cast<System*>(arg);

    while (1) {
        
        std::cout << "entrou thread CalculateRef" << endl;

        int result = local_sys->receiveFrommyQueue(received_data);

        if (result == 0) {  // Verificar se a mensagem tem o tamanho correto

            pthread_mutex_lock(&local_sys->mutex_ReadSensors_msg);

            buffer_temperature.push_back(received_data.temperature);
            buffer_humidity.push_back(received_data.air_humidity);
            buffer_soilhumidity.push_back(received_data.soil_humidity);
            buffer_waterlevel.push_back(received_data.water_level);
            buffer_light.push_back(received_data.light_level);

            pthread_mutex_unlock(&local_sys->mutex_ReadSensors_msg);

            if(buffer_temperature.size() == 10 && buffer_temperature.size() == 10 && buffer_temperature.size() == 10
              && buffer_temperature.size() == 10 && buffer_temperature.size() == 10) {

            send_average_data.timestamp = received_data.timestamp;                  //são as horas com que a ultima a amostra foi retirada
            send_average_data.temperature = calculateAverage(buffer_temperature);
            send_average_data.air_humidity = calculateAverage(buffer_humidity);
            send_average_data.soil_humidity = calculateAverage(buffer_soilhumidity);
            send_average_data.water_level = calculateAverage(buffer_waterlevel);
            send_average_data.light_level = calculateAverage(buffer_light);

            //LÓGICA DE LIGAR/DESLIGAR ATUADORES E ENVIAR ESSES DADOS PARA THREAD ATUADORES POR MESSAGE QUEUE
                pthread_mutex_lock(&local_sys->mutex_Database);
                define_data defined_values;
                defined_values = local_sys->get_define_data();
                pthread_mutex_unlock(&local_sys->mutex_Database);
            
                pthread_mutex_lock(&local_sys->mutex_Actuators);

                buffer_calculate_values[0] = local_sys->relay_module.reference_calculator(send_average_data.temperature, defined_values.define_temperature, TEMP_MARGIN);
                buffer_calculate_values[1] = local_sys->relay_module.reference_calculator(send_average_data.air_humidity, defined_values.define_humidity, HUM_MARGIN);
                buffer_calculate_values[2] = local_sys->relay_module.reference_calculator(send_average_data.soil_humidity, defined_values.define_soilhumidity, HUMSOIL_MARGIN);
                buffer_calculate_values[3] = local_sys->relay_module.reference_calculator(send_average_data.light_level, LIGHT_DEFINE, LIGHT_MARGIN);
                if(send_average_data.water_level < CRITIC_WATERLEVEL){

                    buffer_calculate_values[4] = 1;
                } else {
                    buffer_calculate_values[4] = 0;
                }

            std::cout << "temp: " << buffer_calculate_values[0] << "  " << send_average_data.temperature << "   " << defined_values.define_temperature << endl;
            std::cout << "hum: " << buffer_calculate_values[1] << endl;
            std::cout << "soilhum: " << buffer_calculate_values[2] << endl;
            std::cout << "light: " << buffer_calculate_values[3] << endl;
            std::cout << "water: " << buffer_calculate_values[4] << endl;
            pthread_mutex_unlock(&local_sys->mutex_Actuators);


            //ssize_t result2 = local_sys->sendToActuators(buffer_calculate_values);
            ssize_t result2;
            if (mq_send(QueueActuators, reinterpret_cast<const char*>(buffer_calculate_values), sizeof(int) * 6, 0) == -1) {
                perror("mq_send");
                ssize_t result2 = 1;
                std::cout << "resultado do envio para a atuadores: " << result2 << endl;
            }
            else{
                ssize_t result2 = 0;
                std::cout << "resultado do envio para a atuadores: " << result2 << endl;
            }
                

            int result = local_sys->sendToDatabase(send_average_data);
            std::cout << "resultado do envio para a Database: " << result << " as horas: " << received_data.timestamp << endl;    

            
            buffer_temperature.clear();
            buffer_humidity.clear();
            buffer_soilhumidity.clear();
            buffer_waterlevel.clear();
            buffer_light.clear();

        }

        } else {
            std::cout << "Invalid message size or no data available" << endl;
        }
        usleep(100000);  // Aguarda por 100ms
    }    
}

void *System::tActuators(void *arg) {

    int received_data [sizeof(int) * 6];
    System *local_sys = static_cast<System*>(arg);

    while (1) {
        std::cout << "entrou thread Atuadores" << endl;

        ssize_t bytesRead;// = local_sys->receiveFromActuators(received_data); 

        if (mq_receive(QueueActuators, reinterpret_cast<char*>(received_data), sizeof(int) * 6, 0) == -1) {
            perror("mq_receive");
            ssize_t bytesRead = -1;
            std::cout << "resultado da receção na atuadores: " << bytesRead << endl;
        }
        else{
            ssize_t bytesRead = 0;
            std::cout << "resultado da receção na atuadores: " << bytesRead << endl;
        }

        if (bytesRead == 0) {  // Verificar se a mensagem tem o tamanho correto

            // Imprimir os valores
            pthread_mutex_lock(&local_sys->mutex_Actuators);

                int temp = received_data[0];
                int hum = received_data[1];
                int soilhum = received_data[2];
                int light = received_data[3];
                int waterlevel = received_data[4];
                int door = received_data[5]; //ver se consigo por porta a atuar nesta thread!

            actuators_state prev_actuators_state;
            prev_actuators_state = local_sys->realtime_actuators_state;

                if(temp == 1 && prev_actuators_state.heater_state != true){
                    local_sys->relay_module.set_Actuators_State("HEATER", true);
                    local_sys->relay_module.set_Actuators_State("FAN", false);
                    local_sys->realtime_actuators_state.heater_state = true;
                    local_sys->realtime_actuators_state.fan_state = false;
                    std::cout << "baixa temperatura! " << endl;
                }
                else if(temp == 2 && prev_actuators_state.heater_state != false){
                    local_sys->relay_module.set_Actuators_State("HEATER", false);
                    local_sys->relay_module.set_Actuators_State("FAN", true);
                    local_sys->realtime_actuators_state.heater_state = false;
                    local_sys->realtime_actuators_state.fan_state = true;
                    std::cout << "alta temperatura! " << endl;
                }
                else{ 
                    std::cout << "temperatura ideal! " << endl; 
                }
                
                if(hum == 1 && prev_actuators_state.water_sprinklers_state != true){
                    local_sys->relay_module.set_Actuators_State("WATER SPRINKLER", true);
                    local_sys->realtime_actuators_state.water_sprinklers_state = true;
                    std::cout << "baixa humidade! " << endl;
                }
                else if(hum == 2 && prev_actuators_state.water_sprinklers_state != false){
                    local_sys->relay_module.set_Actuators_State("WATER SPRINKLER", false);
                    local_sys->realtime_actuators_state.water_sprinklers_state = false;
                    std::cout << "alta humidade! " << endl;
                }
                else{ 
                    std::cout << "humidade ideal" << endl; 
                }

                if(waterlevel == 1){
                    local_sys->relay_module.set_Actuators_State("WATERING", false);
                    local_sys->realtime_actuators_state.watering_state = false;
                    std::cout << "falta de àgua -> abasteça o tanque" << endl;
                }
                else if(soilhum == 1 && prev_actuators_state.watering_state != true){
                    local_sys->relay_module.set_Actuators_State("WATERING", true);
                    local_sys->realtime_actuators_state.watering_state = true;
                    std::cout << "solo seco -> ativar rega" << endl;
                }
                else if(soilhum == 2 && prev_actuators_state.watering_state != false){
                    local_sys->relay_module.set_Actuators_State("WATERING", false);
                    local_sys->realtime_actuators_state.watering_state = false;
                    std::cout << "solo humido -> desativar rega" << endl;
                }
                else{ 
                    cout << "solo com humidade ideal!" << endl; 
                }

                if(light == 1 && prev_actuators_state.light_state != true){
                    local_sys->relay_module.set_Actuators_State("LIGHT", true);
                    local_sys->realtime_actuators_state.light_state = true;
                    std::cout << "luz insuficiente -> ligar lampadas" << endl;
                }
                else if(light == 2 && prev_actuators_state.light_state != false){
                    local_sys->relay_module.set_Actuators_State("LIGHT", false);
                    local_sys->realtime_actuators_state.light_state = false;
                    std::cout << "luz suficiente -> desligar lampadas" << endl;
                }
                else{ 
                    std::cout << "luz ideal! " << endl;
                }

                if(door == 1 && prev_actuators_state.door_state != true){       //porta deve ser aberta e não está
                  //  receivedSignal = 0;
                    // Configurar um alarme de 5 segundos
                  //  alarm(5);
                    local_sys->relay_module.set_Actuators_State("DOOR", true);               //abrir a porta
                    local_sys->realtime_actuators_state.door_state = true;
                    std::cout << "abrir porta!" << endl;
                }
                else if(door == 2 && prev_actuators_state.door_state != false){     //porta deve ser fechada e não está
                  //  receivedSignal = 0;
                    local_sys->relay_module.set_Actuators_State("DOOR", false);             //fechar porta 
                    local_sys->realtime_actuators_state.door_state = false;
                    std::cout << "fechar porta!" << endl;
                }
                else{ 
                    std::cout << "não é necessário mexer na porta ideal! " << endl;
                }

                pthread_mutex_unlock(&local_sys->mutex_Actuators);

        } else if (bytesRead == -1) {
            perror("Erro ao receber mensagem na fila de atuadores");
        } else {
            std::cout << "Tamanho inválido da mensagem ou nenhum dado disponível" << endl;
        }

        usleep(100000);  // Aguarda por 100ms
    }
}

void *System::tDatabase(void *arg){

    System *local_sys = static_cast<System*>(arg);
    sensors_data receivedDatabase_data;
    
    while (1) {
        std::cout << "entrou thread Database" << endl;

        int status = local_sys->receiveFromDatabase(receivedDatabase_data);

        if (status == 0) {  // Verificar se a mensagem tem o tamanho correto

            // Imprimir os valores
            std::cout << "Received data:" << endl;
            std::cout << "hours: " << receivedDatabase_data.timestamp << endl;
            std::cout << "Temperature: " << receivedDatabase_data.temperature << endl;
            std::cout << "Air Humidity: " << receivedDatabase_data.air_humidity << endl;
            std::cout << "Soil Humidity: " << receivedDatabase_data.soil_humidity << endl;
            std::cout << "Water Level: " << receivedDatabase_data.water_level << endl;
            std::cout << "Light Level: " << receivedDatabase_data.light_level << endl;

            bool light_state;
            if(receivedDatabase_data.light_level > LIGHT_DEFINE){
                light_state = true;
            }
            else 
                light_state = false;

            pthread_mutex_lock(&local_sys->mutex_Database);
            string time = receivedDatabase_data.timestamp;
            float temperature = receivedDatabase_data.temperature;
            float humidity_air = receivedDatabase_data.air_humidity;
            float soil_humidity = receivedDatabase_data.soil_humidity;
            float water_level = receivedDatabase_data.water_level;

            local_sys->Greenhouse_Database->insertData(time, temperature, humidity_air, soil_humidity, water_level, light_state);
            pthread_mutex_unlock(&local_sys->mutex_Database);
        } else {
            std::cout << "Invalid message size or no data available" << endl;
        }
        usleep(100000);  // Aguarda por 100ms
    }
}

void *System::tsendRemote(void *arg){

    System *local_sys = static_cast<System*>(arg);
    float receiveBuffer[4];

    while(1){
        cout << "Entrou na thread que vai receber os dados!!!" << endl;

        //ssize_t result = mq_send(local_sys->QueuereceiveRemote, reinterpret_cast<const char*>(recieveBuffer), sizeof(int) * 6, 0);

        ssize_t bytesRead;  // = local_sys->receiveFromActuators(received_data); 

        if (mq_receive(local_sys->QueuereceiveRemote, reinterpret_cast<char*>(receiveBuffer), sizeof(float)* 4, 0) == -1) {
            perror("mq_receive");
            ssize_t bytesRead = -1;
            std::cout << "Data recieved on Process 1: " << bytesRead << endl;
        }
        else{
            ssize_t bytesRead = 0;
            std::cout << "Data recieved on Process 1: " << bytesRead << endl;

            bool light_def = false;
            if(receiveBuffer[3] == 1){
                light_def = true;
            }

            local_sys->Greenhouse_Database->insertDefineValues(receiveBuffer[0],receiveBuffer[1],receiveBuffer[2], light_def);

            std::cout << "Received Data: ";
            for (int i = 0; i < 4; ++i) {
                std::cout << receiveBuffer[i] << " ";
            }
            std::cout << std::endl;
        }

/*
        ssize_t result = mq_send(local_sys->QueuesendRemote, reinterpret_cast<const char*>(sendbuffervalues), sizeof(int) * 6, 0);

        if (result == -1) {
            std::cerr << "Error sending data from Process 1" << std::endl;
            perror("mq_send");
        } else {
            std::cout << "Data sent from Process 1" << std::endl;
        }
*/    
    }
}
