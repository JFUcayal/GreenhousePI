#include <iostream>
#include <stdio.h>
#include <string.h>
#include "buff.h"

using namespace std;


Avg_buff::Avg_buff(){

}

Avg_buff::~Avg_buff(){
}

void Avg_buff::init(){
    memset(data, 0, ARRAY_SIZE*4);  //float == 4 bytes
    index_write = 0;
    index_read = 0;
}

void Avg_buff::reset(){
    memset(data, 0, ARRAY_SIZE*4);  //float == 4 bytes
    index_write = 0;
    index_read = 0;
}

void Avg_buff::push(float value){
    if(index_write <= ARRAY_SIZE-1){     //buffer available
        data[index_write] = value;
        index_write++;
    } else {
        cout << "#############################" << endl;
        cout << "Buffer isnt available ---> BUFF_FULL" << endl;
    }
}

float Avg_buff::pop(){
    float value;

    if(index_read <= ARRAY_SIZE-1){
        value = data[index_read];
        //data[index_read] = 0;         //limpar buffer apos leitura
        index_read++;
    } 

    return value;
}

uint16_t Avg_buff::get_index_wr(){
    
    return index_write;
}

uint16_t Avg_buff::get_index_rd(){
    
    return index_read;
}

bool Avg_buff::check_full(){

    uint16_t w = get_index_wr();

    if(w == ARRAY_SIZE){
        cout << "#############################" << endl;
        cout << "BUFFER FULL!" << endl;
        return true;
    }
    else {
        cout << "#############################" << endl;
        cout << "BUFFER ISNT FULL!" << endl; 
        return false;
    }   
}

void Avg_buff::print_buff(){
    for(int i=0; i < ARRAY_SIZE; i++)
        cout << data[i] << " -> ";
    cout << "EOBuff" << endl;
}
