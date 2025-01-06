#ifndef ACTUATORS_H_
#define ACTUATORS_H_

#include <iostream>
#include <string>

using namespace std;

class Actuators{
    private:
        string name;
        bool state;
        int ficheiro;

    public:
        Actuators();        //construtor
        ~Actuators();       //desconstrutor
        void set_Actuators_State(string, bool);     //altera estado dos atuadores
        bool get_Actuators_State(string);           //devolve o estado dos atuadores
        int reference_calculator(float, float, float);  //calcula o desvio entre os valores na estufa e os valores pretendidos
        void initialize_Actuators();                    //inicializa todos os atuadores (como desligados)
        void control_Door(bool);                        //função utilizada para controlar a porta de entrada
};


#endif
