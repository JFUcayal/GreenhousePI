#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h> 

class Database{

public:
    Database(const char* dbName);       //construtor -> abre Database e vê se conexão foi conseguida
    ~Database();                        //destrutor -> fehca a Database

    void insertData(int, float, float, float, float, bool);
    void deleteAllData();
    void showAllData();

private:
    sqlite3* db;

};

#endif