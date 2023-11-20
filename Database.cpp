#include <stdlib.h>
#include <iostream>
#include <sqlite3.h> 

#include "Database.h"

using namespace std;

//constructor -> open Database -> do the connection fase
Database::Database(const char* dbName){

   //OPEN DATABASE
   if( sqlite3_open(dbName, &db) != SQLITE_OK) {
      cerr << "Error open DB " << sqlite3_errmsg(db) << endl; 

   } else {
      cout << "Opened Database Successfully!" << endl;
   }
}

//destructor -> close database
Database::~Database(){

    sqlite3_close(db);
}

//insert data into database -> all data are insert into database
void Database::insertData(int id, float avgTemperature, float avgHumidity, float avgSoil_Humidity, float avgWater_Level, bool luminosity_State){

   sqlite3_stmt *stmt;

   char* sql = "INSERT INTO DATA(ID, avgTemperature, avgHumidity, avgSoil_Humidity,avgWater_Level, luminosity_State) "  \
               "VALUES(?, ?, ?, ?, ?, ?);";

   int rc;
   
   rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   
   sqlite3_bind_int(stmt, 1, id);
   sqlite3_bind_double(stmt, 2, avgTemperature);
   sqlite3_bind_double(stmt, 3, avgHumidity);
   sqlite3_bind_double(stmt, 4, avgSoil_Humidity);
   sqlite3_bind_double(stmt, 5, avgWater_Level);
   sqlite3_bind_int(stmt, 6, luminosity_State);

   sqlite3_step(stmt);
   sqlite3_finalize(stmt);
   
   if( rc != SQLITE_OK) {
      cerr << "Error WRITING DB " << sqlite3_errmsg(db) << endl; 

   } else {
      cout << "DATA SUCCESSFULLY INTRODUCED!" << endl;
   }

}

//delete all database data
void Database::deleteAllData(){        

   const char* deleteSQL = "DELETE FROM DATA;";
   int rc;

   rc = sqlite3_exec(db, deleteSQL, 0, 0, 0);

   if (rc != SQLITE_OK) {
        cerr << "Erro ao apagar dados da tabela: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Dados da tabela apagados com sucesso!" << endl;
    }

}

//function callback used to print the data from database
int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << "\t";
    }
    cout << endl;
    return (0);
}

//function used to print the database data
void Database::showAllData(){

   const char* selectSQL = "SELECT * FROM DATA";
   int rc;

    rc = sqlite3_exec(db, selectSQL, callback, 0, 0);
    if (rc != SQLITE_OK) {
        cerr << "Erro ao selecionar dados da tabela: " << sqlite3_errmsg(db) << endl;
    }

}
