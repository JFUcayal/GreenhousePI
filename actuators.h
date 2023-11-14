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
        //Actuators(string name, bool state);
        Actuators();
        ~Actuators();
        void set_Actuators_State(bool state);
        bool get_Actuators_State();
        uint8_t reference_Calculator(float sample_val, float ref_val, float define_margin);
        void control_Door(bool open);

};

#endif