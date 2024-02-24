#include <stdlib.h>
#include <iostream>
#include <sqlite3.h> 
#include <sstream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <cstring>

#include "Database.h"

using namespace std;

//constructor -> open Database -> do the connection fase
Database::Database(const char* dbName) : db(nullptr) {

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


double roundToTwoDecimals(double value) {
    return round(value * 100) / 100.0;
}

void Database::insertDefineValues(float define_temperature, float define_humidity, float define_soilHumidity, bool light){

   sqlite3_stmt *stmt;

   char* sql = "INSERT INTO DEFINE_VALUES(define_temperature, define_humidity, define_soilHumidity, light) "  \
               "VALUES(?, ?, ?, ?);";

   int rc;
   
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);    //database, sql pedido, -1???, ponteiro para instrução será armazenada, ponteiro para dados não processados

   //sqlite3_bind_int(stmt, 1, ID);
   double defined_roundedTemperature = roundToTwoDecimals(define_temperature);
   sqlite3_bind_double(stmt, 1, defined_roundedTemperature);

   double defined_roundedhumidity = roundToTwoDecimals(define_humidity);
   sqlite3_bind_double(stmt, 2, defined_roundedhumidity);

   double defined_roundedsoilHumidity = roundToTwoDecimals(define_soilHumidity);
   sqlite3_bind_double(stmt, 3, defined_roundedsoilHumidity);

   sqlite3_bind_int(stmt, 4, light);

   rc = sqlite3_step(stmt);
   sqlite3_finalize(stmt);
   
      if (rc != SQLITE_DONE) {
      cerr << "Error WRITING DB (" << rc << "): " << sqlite3_errmsg(db) << endl;
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


bool Database::login(string username, string password){

    sqlite3_stmt *stmt;
    const char* sql = "SELECT * FROM USER WHERE username = ?;";
    int rc;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Bind do parâmetro da consulta
    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        std::cerr << "Error binding parameter: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Executar a consulta
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
         // Usuário encontrado, verifique a senha
         std::string storedPassword(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
         if (storedPassword == password) {
            // Senha corresponde, login bem-sucedido
            std::cout << "Login successful!" << std::endl;
            sqlite3_finalize(stmt);
            return true;
         } else {
            // Senha não corresponde
            std::cout << "Incorrect password!" << std::endl;
            sqlite3_finalize(stmt);
            return false;
         }
    } else {
        // Usuário não encontrado
        std::cout << "User not found!" << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
}


//insert data into database -> all data are insert into database -> DATA TABLE
void Database::insertData(string sampletime, float avgTemperature, float avgHumidity, float avgSoil_Humidity, float avgWater_Level, bool luminosity_State){

   sqlite3_stmt *stmt;

   char* sql = "INSERT INTO DATA(sampleTime, avgTemperature, avgHumidity, avgSoil_Humidity, avgWater_Level, luminosity_State) "  \
               "VALUES(?, ?, ?, ?, ?, ?);";

   int rc;
   
  // rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);    //database, sql pedido, -1???, ponteiro para instrução será armazenada, ponteiro para dados não processados

      rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
      if (rc != SQLITE_OK) {
         cerr << "Error in sqlite3_prepare_v2: " << sqlite3_errmsg(db) << endl;
         // Trate o erro conforme necessário
      }


    const char* sampletimeCStr = sampletime.c_str();

    // Bind da coluna DATETIME
   sqlite3_bind_text(stmt, 1, sampletimeCStr, -1, SQLITE_STATIC);

   double roundedTemperature = roundToTwoDecimals(avgTemperature);
   sqlite3_bind_double(stmt, 2, roundedTemperature);

   double roundedHumidity = roundToTwoDecimals(avgHumidity);
   sqlite3_bind_double(stmt, 3, roundedHumidity);

   double roundedSoil_Humidity = roundToTwoDecimals(avgSoil_Humidity);
   sqlite3_bind_double(stmt, 4, roundedSoil_Humidity);

   double roundedavgWater_Level = roundToTwoDecimals(avgWater_Level);
   sqlite3_bind_double(stmt, 5, roundedavgWater_Level);

   sqlite3_bind_int(stmt, 6, luminosity_State);

   int stepResult = sqlite3_step(stmt);
   if (stepResult != SQLITE_DONE) {
      cerr << "Error in sqlite3_step: " << sqlite3_errmsg(db) << endl;
   }
   sqlite3_finalize(stmt);
   
   if( rc != SQLITE_OK) {
      cerr << "Error WRITING DB " << sqlite3_errmsg(db) << endl; 

   } else {
      cout << "DATA SUCCESSFULLY INTRODUCED!" << endl;
   }

}


/*define_data Database::getDefineValues(){

      define_data data;
      sqlite3_stmt *stmt;
      const char *sql = "SELECT define_temperature, define_humidity, define_soilHumidity, light FROM DEFINE_VALUES ORDER BY ID DESC LIMIT 1;";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                // Recupere o valor da coluna (supondo que seja um número de ponto flutuante)
                float temperatura = data.define_temperature = static_cast<float>(sqlite3_column_double(stmt, 0));
                float humidity = data.define_humidity = static_cast<float>(sqlite3_column_double(stmt, 1));
                float soilhumidity = data.define_soilhumidity = static_cast<float>(sqlite3_column_double(stmt, 2));
                bool light = data.define_light = static_cast<bool>(sqlite3_column_int(stmt, 3));

                cout << "ultimo valor de define temperatura: " << temperatura << endl;
                cout << "ultimo valor de define humidade: " << humidity << endl;
                cout << "ultimo valor de define humidade do solo: " << soilhumidity << endl; 
                cout << "ultimo valor de define luz: " << light << endl;

            } else {
                std::cerr << "Nenhum resultado encontrado." << std::endl;
            }

            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Erro ao preparar a consulta." << std::endl;
        }
        sqlite3_close(db);
        return data;
}
*/

/*sensors_data Database::getDataValues() {

    sensors_data data;
    sqlite3_stmt *stmt;
    const char *sql = "SELECT sampleTime, avgTemperature, avgHumidity, avgSoil_Humidity, avgWater_Level, luminosity_State FROM DATA ORDER BY sampleTime DESC LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // Recupere a string da coluna sampleTime
            const char *sampleTimeString = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            
            // Recupere o valor da coluna (supondo que seja um número de ponto flutuante)
          ///  data.timestamp = sampleTimeString;

            data.timestamp = string(sampleTimeString);;
            data.temperature = static_cast<float>(sqlite3_column_double(stmt, 1));
            data.air_humidity = static_cast<float>(sqlite3_column_double(stmt, 2));
            data.soil_humidity = static_cast<float>(sqlite3_column_double(stmt, 3));
            data.water_level = static_cast<float>(sqlite3_column_double(stmt, 4));
            data.light_level = static_cast<int>(sqlite3_column_int(stmt, 5));

            // Imprima os valores
            cout << "Timestamp: " << data.timestamp << endl;
            cout << "Temperature: " << data.temperature << endl;
            cout << "Air Humidity: " << data.air_humidity << endl;
            cout << "Soil Humidity: " << data.soil_humidity << endl;
            cout << "Water Level: " << data.water_level << endl;
            cout << "Light Level: " << data.light_level << endl;
        } else {
            std::cerr << "Nenhum resultado encontrado." << std::endl;
        }

        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Erro ao preparar a consulta." << std::endl;
    }
    sqlite3_close(db);
    return data;
}
*/
/*
void Database::getDefineValues(define_data& data) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT define_temperature, define_humidity, define_soilHumidity, light FROM DEFINE_VALUES ORDER BY ID DESC LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // Recupere o valor da coluna (supondo que seja um número de ponto flutuante)
            data.define_temperature = static_cast<float>(sqlite3_column_double(stmt, 0));
            data.define_humidity = static_cast<float>(sqlite3_column_double(stmt, 1));
            data.define_soilhumidity = static_cast<float>(sqlite3_column_double(stmt, 2));
            data.define_light = static_cast<bool>(sqlite3_column_int(stmt, 3));

            std::cout << "ultimo valor de define temperatura: " << data.define_temperature << std::endl;
            std::cout << "ultimo valor de define humidade: " << data.define_humidity << std::endl;
            std::cout << "ultimo valor de define humidade do solo: " << data.define_soilhumidity << std::endl;
            std::cout << "ultimo valor de define luz: " << data.define_light << std::endl;
        } else {
            std::cerr << "Nenhum resultado encontrado." << std::endl;
        }

        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Erro ao preparar a consulta." << std::endl;
    }
}

void Database::getDataValues(sensors_data& data) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT sampleTime, avgTemperature, avgHumidity, avgSoil_Humidity, avgWater_Level, luminosity_State FROM DATA ORDER BY sampleTime DESC LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* sampleTimeString = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

            data.timestamp = std::string(sampleTimeString);
            data.temperature = static_cast<float>(sqlite3_column_double(stmt, 1));
            data.air_humidity = static_cast<float>(sqlite3_column_double(stmt, 2));
            data.soil_humidity = static_cast<float>(sqlite3_column_double(stmt, 3));
            data.water_level = static_cast<float>(sqlite3_column_double(stmt, 4));
            data.light_level = static_cast<int>(sqlite3_column_int(stmt, 5));

            std::cout << "Timestamp: " << data.timestamp << std::endl;
            std::cout << "Temperature: " << data.temperature << std::endl;
            std::cout << "Air Humidity: " << data.air_humidity << std::endl;
            std::cout << "Soil Humidity: " << data.soil_humidity << std::endl;
            std::cout << "Water Level: " << data.water_level << std::endl;
            std::cout << "Light Level: " << data.light_level << std::endl;
        } else {
            std::cerr << "Nenhum resultado encontrado." << std::endl;
        }

        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Erro ao preparar a consulta." << std::endl;
    }
}
*/

// Função para obter os últimos valores da tabela define_values
void Database::getDefineValues(float& define_temperature, float& define_humidity, float& define_soilhumidity, bool& define_light) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT define_temperature, define_humidity, define_soilhumidity, light FROM DEFINE_VALUES ORDER BY ID DESC LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // Recupere os valores da coluna e atribua às variáveis de referência
            define_temperature = static_cast<float>(sqlite3_column_double(stmt, 0));
            define_humidity = static_cast<float>(sqlite3_column_double(stmt, 1));
            define_soilhumidity = static_cast<float>(sqlite3_column_double(stmt, 2));
            define_light = static_cast<bool>(sqlite3_column_int(stmt, 3));

            std::cout << "Último valor de define temperatura: " << define_temperature << std::endl;
            std::cout << "Último valor de define humidade: " << define_humidity << std::endl;
            std::cout << "Último valor de define humidade do solo: " << define_soilhumidity << std::endl;
            std::cout << "Último valor de define luz: " << define_light << std::endl;
        } else {
            std::cerr << "Nenhum resultado encontrado." << std::endl;
        }

        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Erro ao preparar a consulta." << std::endl;
    }
}

// Função para obter os últimos valores da tabela lidos pelos sensores para enviar para aplicação
void Database::getDataValues(std::string& timestamp, float& temperature, float& air_humidity, float& soil_humidity, float& water_level, float& light_level) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT sampleTime, avgTemperature, avgHumidity, avgSoil_Humidity, avgWater_Level, luminosity_State FROM DATA ORDER BY sampleTime DESC LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* sampleTimeString = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

            timestamp = std::string(sampleTimeString);
            temperature = static_cast<float>(sqlite3_column_double(stmt, 1));
            air_humidity = static_cast<float>(sqlite3_column_double(stmt, 2));
            soil_humidity = static_cast<float>(sqlite3_column_double(stmt, 3));
            water_level = static_cast<float>(sqlite3_column_double(stmt, 4));
            light_level = static_cast<int>(sqlite3_column_int(stmt, 5));

            std::cout << "Timestamp: " << timestamp << std::endl;
            std::cout << "Temperature: " << temperature << std::endl;
            std::cout << "Air Humidity: " << air_humidity << std::endl;
            std::cout << "Soil Humidity: " << soil_humidity << std::endl;
            std::cout << "Water Level: " << water_level << std::endl;
            std::cout << "Light Level: " << light_level << std::endl;
        } else {
            std::cerr << "Nenhum resultado encontrado." << std::endl;
        }

        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Erro ao preparar a consulta." << std::endl;
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
