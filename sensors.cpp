#include "sensors.h"
#include <iostream>

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdint>

using namespace std;

const char* i2c_device = "/dev/i2c-1";
const int i2c_address = 0x40;            //si7021 addr
int i2c_file;          //file descriptor -> open i2c file in Read/Write mode

/// @brief constructor of temp and hum sensor and reset buffers
Temp_Hum_Sensor::Temp_Hum_Sensor(){
    cout << "****************************************" << endl;
    cout << "Temperature & Humidity sensor created! " << endl;

    //initialize the buffers
    temp_sample_buff.init();
    hum_sample_buff.init();
}

/// @brief destructor
Temp_Hum_Sensor::~Temp_Hum_Sensor(){
    cout << "****************************************" << endl;
    cout << "Temperature & Humidity sensor destroyed! " << endl;
}

/// @brief set up the i2c device
void Temp_Hum_Sensor::init(){

    i2c_file = open(i2c_device, O_RDWR); 

    if(i2c_file == -1){
        cerr << "#############################" << endl;
        cerr << "Error opening i2c file!" << endl;
        return;
    }

    if (ioctl(i2c_file, I2C_SLAVE, i2c_address) < 0){
        cerr << "#############################" << endl;
        cerr << ("Error in i2c device!");
        close(i2c_file);
        return;
    } 
}

/// @brief temperature measurement
/// @return temperature sample 
float Temp_Hum_Sensor::get_Temperature(){
    const int tempAddr = 0xE3;      //Measure CMD
    uint8_t temp_buff[2] = {0};     //MSByte + LSByte 
    float sample_temperature = 0;
    static int sample_count = 0;
    uint16_t auxTemp = 0;

    init();    //open and setup i2c device
    
    if (write(i2c_file, &tempAddr, 1) != 1) {       //Master -> measure cmd
        cerr << "#############################" << endl;
        cerr << "Error in setup!" << endl;
        close(i2c_file);
        return 0;       
    }

    usleep(75000);

    if (read(i2c_file, temp_buff, 2) != 2){         //ERR -> 2 bytes not returned
        cerr << "#############################" << endl;
        cerr << "Error reading temperature data!" << endl;
        close(i2c_file);
        return 0;
    }

    close(i2c_file);

    //Concatenation of temp value -> MSByte(tempbuff[0] shifted 8 bits to the left followed by ORL with LSByte(temp_buff[1]))
    auxTemp = (temp_buff[0] << 8 | temp_buff[1]);  
    sample_temperature = (175.72 * auxTemp / 65536.0) - 46.85;

    cout << "****************************************" << endl;
    cout << "Sample Temperature: " << sample_temperature << "°C" << endl;

    //store sample value in array for future average calculations
    temp_sample_buff.push(sample_temperature);
    cout << "******** TEMP BUFFER PRINT ********" << endl;
    temp_sample_buff.print_buff();

    //signal when buffer is full for calculation
    if(temp_sample_buff.check_full()){

        //Conditional variable for ref calc -> send temp_sample_buf to system
        
    } else {
        cout << "****************************************" << endl;
        cout << "Temperature value added to array!" << endl;
    }

    return sample_temperature;
}

/// @brief air humidity measurement
/// @return humidity sample
float Temp_Hum_Sensor::get_Humidity(){
    const int humAddr = 0xE5;
    float sample_humidity = 0;
    static int sample_count = 0; 
    uint8_t temp_buff[2] = {0};     //MSByte + LSByte  
    uint16_t auxHum = 0;
    
    init();

        if (write(i2c_file, &humAddr, 1) != 1) {       //Master -> measure cmd
        cerr << "#############################" << endl;
        cerr << "Error in setup!" << endl;
        close(i2c_file);
        return 0;       
    }

    usleep(75000);

    if (read(i2c_file, temp_buff, 2) != 2){         //ERR -> 2 bytes not returned
        cerr << "#############################" << endl;
        cerr << "Error reading humidity data!" << endl;
        close(i2c_file);
        return 0;
    }

    close(i2c_file);

    //Concatenation of temp value -> MSByte(tempbuff[0] shifted 8 bits to the left followed by ORL with LSByte(temp_buff[1]))
    auxHum = (temp_buff[0] << 8 | temp_buff[1]);  
    sample_humidity = (125.0 * auxHum / 65536.0) - 6.0;

    cout << "****************************************" << endl;
    cout << "Sample Humidity: " << sample_humidity << "%" << endl;

    //store sample value in array for future average calculations
    hum_sample_buff.push(sample_humidity);
    cout << "******** HUM BUFFER PRINT ********" << endl;
    hum_sample_buff.print_buff();

    //signal when buffer is full for calculation
    if(hum_sample_buff.check_full()){

        //Conditional variable for ref calc -> send hum_sample_buf to system

    } else {
        cout << "****************************************" << endl;
        cout << "Humidity value added to array!" << endl;
    }

    

    return sample_humidity;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

Soil_Hum_Sensor::Soil_Hum_Sensor(){

}

Soil_Hum_Sensor::~Soil_Hum_Sensor(){

}

float Soil_Hum_Sensor::get_soil_moisture(){

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

LDR_Sensor::LDR_Sensor(){

}

LDR_Sensor::~LDR_Sensor(){

}

float LDR_Sensor::get_brightness_value(){

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

Water_Level_Sensor::Water_Level_Sensor(){

}

Water_Level_Sensor::~Water_Level_Sensor(){

}

float Water_Level_Sensor::get_water_level(){

}