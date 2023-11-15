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

/// @brief receber dados e enviar 
void System::data_processing(){
    //insert data 
    
}

time_t System::get_time(){

    time_t curr_time;
	curr_time = time(NULL);

	char *tm = ctime(&curr_time);
    cout << "****************************************" << endl;
	cout << "Time: " << tm;
	
	return *tm;
}

int System::get_ID(){

    return ID;
}