#include "logger.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;


[[noreturn]] void erro(std::string message){
    cerr << "ERRO: " <<  message << endl;
    exit(EXIT_FAILURE);
}



void aviso(std::string message){
    cerr << "MUITO CUIDADO -> AVISO: " << message << endl;
}
