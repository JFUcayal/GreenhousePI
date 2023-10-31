#ifndef ACTUATORS_H_
#define ACTUATORS_H_

#include <iostream>
#include <string>

using namespace std;

class Actuators{
    private:
        string name;
        bool state;

    public:
        Actuators(string name, bool state);
        ~Actuators();
        void set_Actuators_State(string name, bool state);
        bool get_Actuators_State();
        void reference_Calculator();
        void control_Door();

};

#endif