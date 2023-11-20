#include "actuators.h"
#include <iostream>

using namespace std;

Actuators::Actuators(/*string A_name, bool A_state*/){

    //name = A_name;
    //state = A_state;

    cout << "****************************************" << endl;
    cout << "Actuator created " << endl; 
}

Actuators::~Actuators(){

    cout << "****************************************" << endl;
    cout << "Actuator destroyed " << endl; 
}

bool Actuators::get_Actuators_State(){

    cout << " -> State: " << state << endl;

    return state;
}

void Actuators::set_Actuators_State(bool set_state){
    state = set_state;
}


uint8_t reference_calculator(float sample_val, float ref_val, float define_margin){
    
    static float prev_state = ref_val;  

    if(sample_val >= prev_state + define_margin){           //above margin
        prev_state = sample_val;    //update values
        cout << "Above margin ---> Activate cooling system" << endl;
        return 2;   //signal to cool system
    } else if(sample_val <= prev_state - define_margin){    //below margin
        prev_state = sample_val;
        cout << "Below margin ---> Activate heating system" << endl;
        return 1;   //signal to heat system
    } else {                                                //in the margin
        cout << "Value respecting margin!" << endl;
        return 0;   //signal to ignore action
    }    
}

void Actuators::control_Door(bool open){

    if(open){
        //open the door for 3s
    } 

}