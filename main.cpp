#include <iostream>
#include <stdio.h>
#include <string>

#include "user.h"
#include "sensors.h"
#include "actuators.h"

using namespace std;

int main(){

    User admin;
    string name = "antonio";
    string pass = "123";

    //Login verification
    if(admin.login(pass)){

        //new parameters
        admin.set_parameters(25, 50, 50, true);

        Actuators bomba_agua1("bomba_rega", false);
        bomba_agua1.get_Actuators_State();

        LDR_Sensor light1(void);
    }

    return 0;
}