#include <iostream>
#include <string>

#include "user.h"
#include "admin.h"

using namespace std;

Admin::Admin(){
    cout << "Admin Created" << endl;
}

Admin::~Admin(){
    cout << "Admin Destroyed" << endl;
}

void Admin::set_parameters(float temperature, float humidity, float soil_moisture, bool light){
    
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

void Admin::add_user(string username){
    //add username to database + pass and name fields (empty)
}

void Admin::remove_user(string username){
    //find username in database and remove 
}