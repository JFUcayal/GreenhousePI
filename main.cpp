#include <iostream>
#include <stdio.h>
#include <string>

#include "user.h"
#include "actuators.h"

using namespace std;


int main(){

    User admin;
    string name = "antonio";
    string pass = "123";

    if(admin.login(pass)){

        admin.set_parameters(25, 50, 50, true);

        Actuators bomba_agua("bomba_H20", false);
        bomba_agua.get_Actuators_State();
    }

    return 0;
}