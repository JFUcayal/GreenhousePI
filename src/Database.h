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

    void insertData(string, float, float, float, float, bool);  //inserir dados lidos pelos sensores
    void insertDefineValues(float, float, float, bool);         //inserir valores pretendidos na estufa
    void insertUser(std::string, bool);                         //insere utilizadores
    void signupUser(string, string, string);
    bool username_exist(string);
    void deleteAllData(string);
    void showAllData(string);
    //float getDefineValues();
  //  sensors_data getDataValues();
//    void getDataValues(sensors_data& data);
//    void getDefineValues(define_data& data);
    void getDefineValues(float&, float&, float&, bool&);
    void getDataValues(string&, float&, float&, float&, float&, float&);
    bool login(string, string);

private:
    sqlite3* db;

};

#endif
