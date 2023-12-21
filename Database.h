#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <stdlib.h>
#include <string>
#include <sqlite3.h> 

using namespace std;

class Database{

public:
    Database(const char* dbName);       //construtor -> abre Database e vê se conexão foi conseguida
    ~Database();                        //destrutor -> fehca a Database

    void insertData(tm, float, float, float, float, bool);
    void insertDefineValues(float, float, float, bool);
    void insertUser(std::string, bool);
    void signupUser(string, string, string);
    bool username_exist(string);
    void deleteAllData(string);
    void showAllData(string);
    
    bool login(string, string);

    /*
        USER.H

        bool login(string userpass);
        void sign_up(string name, string pass, int ID);
        void set_parameters(float temperature, float humidity, float soil_moisture, bool light);
        void add_user(string username, bool token);
        void remove_user(string username);
    */

private:
    sqlite3* db;

};

#endif