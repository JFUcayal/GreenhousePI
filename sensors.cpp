#include "sensors.h"
#include <iostream>

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdint>

using namespace std;

#define THRESHOLD 100

int i2c_file;          //file descriptor -> open i2c file in Read/Write mode
const char* i2c_device = "/dev/i2c-1";

const int i2c_address = 0x40;            //si7021 addr

const int waterAddr_L = 0x77;
const int waterAddr_H = 0x78;


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


    //signal when buffer is full for calculation
    if(temp_sample_buff.check_full() == 1){

        //Conditional variable for ref calc -> send temp_sample_buf to system
        cout << "buffer ready for calculation! " << endl;

    } else {

        //Concatenation of temp value -> MSByte(tempbuff[0] shifted 8 bits to the left followed by ORL with LSByte(temp_buff[1]))
        auxTemp = (temp_buff[0] << 8 | temp_buff[1]);  
        sample_temperature = (175.72 * auxTemp / 65536.0) - 46.85;

        cout << "****************************************" << endl;
        cout << "Sample Temperature: " << sample_temperature << "°C" << endl;

        //store sample value in array for future average calculations
        temp_sample_buff.push(sample_temperature);
        cout << "******** TEMP BUFFER PRINT ********" << endl;
        temp_sample_buff.print_buff();

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

    

    //signal when buffer is full for calculation
    if(hum_sample_buff.check_full() == 1){

        //Conditional variable for ref calc -> send hum_sample_buf to system
        cout << "buffer ready for calculation! " << endl;

    } else {

        //Concatenation of temp value -> MSByte(tempbuff[0] shifted 8 bits to the left followed by ORL with LSByte(temp_buff[1]))
        auxHum = (temp_buff[0] << 8 | temp_buff[1]);  
        sample_humidity = (125.0 * auxHum / 65536.0) - 6.0;

        cout << "****************************************" << endl;
        cout << "Sample Humidity: " << sample_humidity << "%" << endl;

        //store sample value in array for future average calculations
        hum_sample_buff.push(sample_humidity);
        cout << "******** HUM BUFFER PRINT ********" << endl;
        hum_sample_buff.print_buff();

        cout << "****************************************" << endl;
        cout << "Humidity value added to array!" << endl;
    }

    

    return sample_humidity;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

Soil_Hum_Sensor::Soil_Hum_Sensor(){
    cout << "****************************************" << endl;
    cout << "Soil Humidity Sensor Created! " << endl;

    soilH_sample_buff.init();
}

Soil_Hum_Sensor::~Soil_Hum_Sensor(){
    cout << "****************************************" << endl;
    cout << "Soil Humidity Sensor Destroyed! " << endl;
}

float Soil_Hum_Sensor::get_soil_moisture(){

    float sample_soil_h;    //ADC value 

    if(soilH_sample_buff.check_full() == 1){
        //Conditional variable for ref calc -> send soil_sample_buf to system
        cout << "buffer ready for calculation! " << endl;
    } else {
        //AIN0 -> ADC input for soil humidity  
        sample_soil_h = get_adc_value(0);
         
        //store sample value in array for future average calculations
        soilH_sample_buff.push(sample_soil_h);
        cout << "******** SOIL BUFFER PRINT ********" << endl;
        soilH_sample_buff.print_buff();

        cout << "****************************************" << endl;
        cout << "Soil Humidity value added to array!" << endl;
    }

    return sample_soil_h;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

LDR_Sensor::LDR_Sensor(){
    cout << "****************************************" << endl;
    cout << "Light Detection Sensor Created! " << endl;
}

LDR_Sensor::~LDR_Sensor(){
    cout << "****************************************" << endl;
    cout << "Light Detection Sensor Created! " << endl;
}

float LDR_Sensor::get_brightness_value(){
    //ADC value

    float sample_bright;

    //AIN1 -> ADC input for brightness reading 
    sample_bright = get_adc_value(1);

    return sample_bright;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

Water_Level_Sensor::Water_Level_Sensor(){
    cout << "****************************************" << endl;
    cout << "Water Level Sensor Created! " << endl;
}

Water_Level_Sensor::~Water_Level_Sensor(){
    cout << "****************************************" << endl;
    cout << "Water Level Sensor Destroyed! " << endl;
}

int Water_Level_Sensor::get_water_level(){

    unsigned char low_data[8] = {0};
    unsigned char high_data[12] = {0};

    uint32_t touch_val = 0;
    uint8_t trig_section = 0; 

    int sample_water = 0;

    i2c_file = open(i2c_device, O_RDWR); 

    if(i2c_file == -1){
        cerr << "#############################" << endl;
        cerr << "Error opening i2c file!" << endl;
        return 0;
    }

    //clear data buffers
    memset(low_data, 0, sizeof(low_data));
    memset(high_data, 0, sizeof(high_data));

    if(ioctl(i2c_file, I2C_SLAVE, waterAddr_L) < 0) {
        cerr << "Erro ao configurar o endereço I2C." << endl;
        close(i2c_file);
        exit(1);
    }

    write(i2c_file, nullptr, 0);  // Start Reading

    usleep(75000);

    if(read(i2c_file, low_data, 8) != 8) {
        cerr << "Erro ao ler dados do sensor." << endl;
        close(i2c_file);
        exit(1);
    }

    if(ioctl(i2c_file, I2C_SLAVE, waterAddr_H) < 0) {
        cerr << "Erro ao configurar o endereço I2C." << endl;
        close(i2c_file);
        exit(1);
    }

    write(i2c_file, nullptr, 0);  // Start Reading

    usleep(75000);

    if (read(i2c_file, high_data, 12) != 12) {
        std::cerr << "Erro ao ler dados do sensor." << std::endl;
        close(i2c_file);
        exit(1);
    }

    usleep(10000);

    for(int i = 0; i < 8; i++) {
        if (low_data[i] > THRESHOLD) {
            touch_val |= 1 << i;
        }
    }

    for(int i = 0; i < 12; i++) {
        if (high_data[i] > THRESHOLD) {
            touch_val |= (uint32_t)1 << (8 + i);
        }
    }

    //walk up to the strip that is not in contact with the water
    while(touch_val & 0x01) {
        trig_section++;
        touch_val >>= 1;
    }

    close(i2c_file);

    sample_water = (trig_section * 5); 

    cout << "Water Level -> " << sample_water << " % " << endl;

    //conversion to percentage -> 20 stripes x 5 -> 100%
    return sample_water;
}