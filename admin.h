#ifndef ADMIN_H_
#define ADMIN_H_

#include <iostream>
#include <string>
#include "user.h"

using namespace std;

class Admin : public User{

    public:
        Admin();
        ~Admin();
        void set_parameters(float temperature, float humidity, float soil_moisture, bool light);

        //Add/remove worker in database
        void add_user(string username);
        void remove_user(string username);
};

#endif