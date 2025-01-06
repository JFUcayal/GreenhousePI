#include "actuators.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "logger.h"

//define dos atuadores
#define FAN 1
#define HEATER 2
#define WATER_SPRINKLER 3
#define WATERING 4
#define LIGHT 5
#define DOOR 6

using namespace std;

Actuators::Actuators(){

    //introduz device driver no programa
    const char *comando = "insmod led.ko";
    int resultado = system(comando);

    // Verificar se a execução foi bem-sucedida
    if (resultado == 0) {
        // O comando foi executado com sucesso
        std::cout << "Módulo do driver carregado com sucesso.\n";
    } else {
        //comando falhou
        aviso("Falha ao carregar o módulo do driver");
    }
}      

Actuators::~Actuators(){

    const char *comando = "rmmod led.ko";
    int resultado = system(comando);

    // Verificar se a execução foi bem-sucedida
    if (resultado == 0) {
        // O comando foi executado com sucesso
        std::cout << "Módulo do driver retirado com sucesso.\n";
    } else {
        //comando falhou
        std::cerr << "Falha ao retirar o módulo do driver.\n";
    }

}

void Actuators::initialize_Actuators() {
    set_Actuators_State("FAN", false);
    set_Actuators_State("HEATER", false);
    set_Actuators_State("WATER_SPRINKLER", false);
    set_Actuators_State("WATERING", false);
    set_Actuators_State("DOOR", false);
    set_Actuators_State("LIGHT", false);
}

/*
void Actuators::initialize_Actuators() {
    set_Actuators_State("FAN", true);
    sleep(1);
    set_Actuators_State("FAN", false);
    sleep(1);
    set_Actuators_State("HEATER", true);
    sleep(1),
    set_Actuators_State("HEATER", false);
    sleep(1);
    set_Actuators_State("WATER_SPRINKLER", true);
    sleep(1);
    set_Actuators_State("WATER_SPRINKLER", false);
    sleep(1);
    set_Actuators_State("WATERING", true);
    sleep(1);
    set_Actuators_State("WATERING", false);
    sleep(1);
    set_Actuators_State("DOOR", true);
    sleep(1);
    set_Actuators_State("DOOR", false);
    sleep(1);
    set_Actuators_State("LIGHT", true);
    sleep(1);
    set_Actuators_State("LIGHT", false);
}
*/

bool Actuators::get_Actuators_State(string actuator){

    return false;
}

void Actuators::set_Actuators_State(string actuator_name, bool set_state) {
    int actuator;

    if (actuator_name == "FAN") {
        actuator = FAN;
    //    cout << "Ventoinha irá ficar no estado: " << set_state << endl;
    } else if (actuator_name == "HEATER") {
        actuator = HEATER;
    //    cout << "HEATER irá ficar no estado: " << set_state << endl;
    } else if (actuator_name == "WATER_SPRINKLER") {
        actuator = WATER_SPRINKLER;
    //    cout << "WATER SPRINKLER irá ficar no estado: " << set_state << endl;
    } else if (actuator_name == "WATERING") {
        actuator = WATERING;
    //    cout << "WATERING irá ficar no estado: " << set_state << endl;
    } else if (actuator_name == "LIGHT") {
        actuator = LIGHT;
    //    cout << "LIGHT irá ficar no estado: " << set_state << endl;
    } else if (actuator_name == "DOOR") {
        actuator = DOOR;
    //    cout << "DOOR irá ficar no estado: " << set_state << endl;
    } else {
        cerr << "Atuador não reconhecido: " << actuator_name << endl;
        return;
    }

    int ficheiro = open("/dev/rele0", O_WRONLY);

    if (ficheiro < 0) {
        erro("Erro ao abrir o dispositivo");
    }

    char comando[3];

    sprintf(comando, "%d%d", actuator, set_state ? 1 : 0);
    write(ficheiro, comando, strlen(comando));
    std::cout << "Atuador " << actuator_name << " irá ficar no estado: " << set_state << endl;

    close(ficheiro);

}

int Actuators::reference_calculator(float sample_val, float ref_val, float define_margin){
    

    if(sample_val >= ref_val + define_margin){           //above margin
        return 2;   //signal to cool system

    } else if(sample_val <= ref_val - define_margin){    //below margin
        return 1;   //signal to heat system
    
    } else {                                                //in the margin
        return 0;   //signal to ignore action
    }    
}

void Actuators::control_Door(bool door_state){
}

