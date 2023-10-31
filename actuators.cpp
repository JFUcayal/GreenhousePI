#include "actuators.h"
#include <iostream>

using namespace std;

Actuators::Actuators(string A_name, bool A_state){

    name = A_name;
    state = A_state;

    cout << "**************************" << endl;
    cout << "Atuador criado: " << name << " Estado: " << state << endl; 
}

Actuators::~Actuators(){

    cout << "**************************" << endl;
    cout << "Atuador destruido: " << name << endl; 

}

bool Actuators::get_Actuators_State(){

    cout << "**************************" << endl;
    cout << name <<"Actuator State: " << state << endl;

    return state;
}

void Actuators::control_Door(){

}