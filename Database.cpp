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

/*
void Database::start(){

   sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0);
}

void Database::close() {
    sqlite3_exec(db, "COMMIT", 0, 0, 0);
}
*/

void Database::insertDefineValues(float define_temperature, float define_humidity, float define_soilHumidity, bool light){

   sqlite3_stmt *stmt;

   char* sql = "INSERT INTO DEFINE_VALUES(define_temperature, define_humidity, define_soilHumidity, light) "  \
               "VALUES(?, ?, ?, ?);";

   int rc;
   
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);    //database, sql pedido, -1???, ponteiro para instrução será armazenada, ponteiro para dados não processados
   
   //sqlite3_bind_int(stmt, 1, ID);
   sqlite3_bind_double(stmt, 1, define_temperature);
   sqlite3_bind_double(stmt, 2, define_humidity);
   sqlite3_bind_double(stmt, 3, define_soilHumidity);
   sqlite3_bind_int(stmt, 4, light);

   rc = sqlite3_step(stmt);
   sqlite3_finalize(stmt);
   
   if( rc != SQLITE_OK) {
      cerr << "Error WRITING DB " << sqlite3_errmsg(db) << endl; 

   } else {
      cout << "DATA SUCCESSFULLY INTRODUCED!" << endl;
   }
}

void Database::signupUser(string username, string name, string password){

sqlite3_stmt *stmt;

// Verifica se o username existe na tabela USER
    if (username_exist(username)) {
        char* sql = "UPDATE USER SET name = COALESCE(name, ?), password = COALESCE(password, ?) WHERE username = ?;";

      sqlite3_stmt *stmt;
      
         sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

         sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
         sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
         sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);

          int rc = sqlite3_step(stmt);

         if (rc != SQLITE_DONE) {
            cerr << "Erro ao executar a atualização do nome e senha na database" << endl;
        } else {
            cout << "Com sucesso username: " << username << " name: " << name << " password: " << password << endl;
        }

        rc = sqlite3_finalize(stmt);
        if (rc != SQLITE_OK) {
               cerr << "Erro ao finalizar a consulta: " << sqlite3_errmsg(db) << endl;
            }
   }  
}

bool Database::username_exist(string username) {

   sqlite3_stmt *stmt;

   char* sql = "SELECT username FROM USER WHERE username = ?;";
   const char *name = nullptr;

   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

   sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
   
   int rc = sqlite3_step(stmt);  //executada até retornar valor que indica que foi concluída

   if(rc == SQLITE_ROW){

      cout << "username: " << username << " encontrado" << endl; 

      rc = sqlite3_finalize(stmt);
      if (rc != SQLITE_OK) {
         cerr << "Erro ao finalizar a consulta: " << sqlite3_errmsg(db) << endl;
         return false;
      }

      return true;
   }
   else if(rc == SQLITE_DONE){

      cout << "username: " << username << " não encontrado" << endl;       
   }
   else {
      cerr << "erro ao executar procura do username na database";
   }

      rc = sqlite3_finalize(stmt);
      if (rc != SQLITE_OK) {
      cerr << "Erro ao finalizar a consulta: " << sqlite3_errmsg(db) << endl;
      }

      return false;


}

void Database::insertUser(string username, bool token){

   sqlite3_stmt *stmt;

   char* sql = "INSERT INTO USER(username, token) "  \
               "VALUES(?, ?);";

   int rc;
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL); 

   sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_int(stmt, 2, token);

   rc = sqlite3_step(stmt);             // executa instrução SQLite preparada previamente compilada usando "sqlite3_prepare_v2()"
   sqlite3_finalize(stmt);
   
   if( rc != SQLITE_DONE) {
      cerr << "Error WRITING DB " << sqlite3_errmsg(db) << endl; 

   } else {
      cout << "USER SUCCESSFULLY INTRODUCED!" << endl;
   }
}



//insert data into database -> all data are insert into database -> DATA TABLE
void Database::insertData(tm sampletime, float avgTemperature, float avgHumidity, float avgSoil_Humidity, float avgWater_Level, bool luminosity_State){

   sqlite3_stmt *stmt;

   char* sql = "INSERT INTO DATA(sampletime, avgTemperature, avgHumidity, avgSoil_Humidity,avgWater_Level, luminosity_State) "  \
               "VALUES(?, ?, ?, ?, ?, ?);";

   int rc;
   
   rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);    //database, sql pedido, -1???, ponteiro para instrução será armazenada, ponteiro para dados não processados
   

   char sampleTimeStr[20]; // Tamanho suficiente para armazenar a data e hora formatadas
      strftime(sampleTimeStr, sizeof(sampleTimeStr), "%d-%m-%Y %H:%M:%S", &sampletime);   //converte a variável time numa string

        // Bind da coluna DATETIME
      sqlite3_bind_text(stmt, 1, sampleTimeStr, -1, SQLITE_STATIC);


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
void Database::deleteAllData(string tableName){        

   string deleteSQL = "DELETE FROM " + tableName + ";";

   int rc;

   rc = sqlite3_exec(db, deleteSQL.c_str(), 0, 0, 0);

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
void Database::showAllData(string table){

   string selectSQL = "SELECT * FROM " + table + ";";
   int rc;

    rc = sqlite3_exec(db, selectSQL.c_str(), callback, 0, 0);
    if (rc != SQLITE_OK) {
        cerr << "Erro ao selecionar dados da tabela: " << sqlite3_errmsg(db) << endl;
    }

}


bool Database::login(string username, string password){

}