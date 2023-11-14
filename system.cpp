#include "system.h"

#include <iostream>
#include <stdio.h>

using namespace std;

System::System(){
    cout << "****************************************" << endl;
    cout << "System Created" << endl ;
}

System::~System(){
    cout << "****************************************" << endl;
    cout << "User Destroyed" << endl ;
}

void System::data_processing(){
    
}

uint16_t adc_convert(){
    static uint16_t adc_value;


    return adc_value;
}