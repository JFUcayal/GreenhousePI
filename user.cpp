#include <iostream>
#include <string>

#include "user.h"

using namespace std;

User::User(){
    cout << "****************************************" << endl;
    cout << "User Created" << endl ;
    name = "admin";
    pass = "123";
}

User::~User(){
    cout << "****************************************" << endl;
    cout << "User Destroyed" << endl;
}

void User::set_parameters(float temperature, float humidity, float soil_moisture, bool light){
    
    float setTemp, setHum, setSoilH;
    bool setLight;


    setTemp = temperature;
    setHum = humidity;
    setSoilH = soil_moisture;
    setLight = light;

    cout << "****************************************" << endl;
    cout << "New Values -> " << "T:"   << setTemp 
                             << " H:"   << setHum 
                             << " S_H:" << setSoilH 
                             << " L:"   << setLight << endl;
}

bool User::login(string userpass){

    cout << "****************************************" << endl;
    if(userpass == pass){
        cout << "ACCESS GRANTED!" << endl;

        return true;

    } else {
        cout << "ACCESS DENIED!" << endl;

        return false;
    }
}

void User::sign_up(string sign_name, string sign_pass, int ID){
 
    
    
}