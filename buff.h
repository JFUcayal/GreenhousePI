#ifndef BUFF_H_
#define BUFF_H_

#include <iostream>
#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE 10

class Avg_buff{
    private:
        float data[ARRAY_SIZE];
        uint8_t index_write, index_read;
    public:
        Avg_buff();
        ~Avg_buff();
        void init();
        void reset();
        void push(float value);
        float pop();
        bool check_full();
        uint16_t get_index_wr();
        uint16_t get_index_rd();
        void print_buff();
        //float get_value(uint8_t index); //retrieve specific value from the array
};

#endif