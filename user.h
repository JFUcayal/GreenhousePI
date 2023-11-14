#ifndef USER_H_
#define USER_H_

#include <iostream>
#include <string>

using namespace std;

class User{
    private:
        string name, pass;

    public:
        User();
        ~User();
        void set_parameters(float temperature, float humidity, float soil_moisture, bool light);
        bool login(string userpass);
};

#endif