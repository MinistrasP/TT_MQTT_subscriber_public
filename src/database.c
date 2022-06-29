#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include <syslog.h>
#include "database.h"

sqlite3 *db;
sqlite3_stmt *res;
const char filepath[] = "/var/log/mqtt.db";

/*
*   Creates a fixed name <mqtt> database
*   With 5 columns
*   Doesnt drop table, on finish closes database
*/
int createDb(void)
{
    char *err_msg;
    openDatabase();

    char *sql = "CREATE TABLE IF NOT EXISTS mqtt"
                "(Id INTEGER PRIMARY KEY, Date TEXT, Time TEXT, topic TEXT, payload TEXT);";
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_close(db);
    syslog(LOG_INFO, "MQTT subsriber: Database created");
    return 0;

}

/*
*   Inserts data into fixed filepath database
*   Data is inserted inside mqtt table
*   There are 2 user generated inputs: topic and payload
*   Id, date, time are automatically generated
*/
int insertData(char *topic, char* payload)  //Inserts single line of data, database needs to be opened/close
{
    char *err_msg;
    char *sql = sqlite3_mprintf("INSERT INTO mqtt(date, time, topic, payload) VALUES (CURRENT_DATE, CURRENT_TIME, '%q', '%q');", topic, payload);
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_free(sql);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_free(err_msg);
    sqlite3_free(sql);
    return 0;
}

/*
*   Prints all data of fixed path database from mqtt table
*/
int printData(void)  
{
    char *err_msg;

    int rc = sqlite3_open(filepath, &db);

    if(rc != SQLITE_OK)
    {
        syslog(LOG_ERR, "MQTT subsriber: Cannot creat database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    char *sql = "SELECT * FROM mqtt";
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    if(rc != SQLITE_OK)
    {
        syslog(LOG_ERR, "MQTT subsriber: Failed to select data SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    
    sqlite3_close(db);
    return 0;
}

/*
*   deleteAllData() used to delete all data from fixed database
*   Automatically opens/closes database
*/
int deleteAllData(void)
{
    char *err_msg;

    int rc = sqlite3_open(filepath, &db);

    if(rc != SQLITE_OK)
    {
        syslog(LOG_ERR, "MQTT subsriber: Cannot open database: %s\n", err_msg);
        sqlite3_close(db);
        return 1;
    }
    char *sql = "DELETE FROM mqtt;";
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if(rc != SQLITE_OK)
    {
        syslog(LOG_ERR, "MQTT subsriber: SQL error - %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    
    sqlite3_close(db);
    return 0;
}

/*
*   callback() is used for function printData()
*/
int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    NotUsed = 0;
    for(int i = 0; i < argc; i++)
    {
        printf(" %s = %s |", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");

    return 0;
}

/*
*   Is used to open database
*   Should be called before data insertion, and after dbCreation if not created
*/
int openDatabase(void)
{
    int rc = sqlite3_open(filepath, &db);

    if(rc != SQLITE_OK)
    {
        syslog(LOG_ERR, "MQTT subsriber: Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    return 0;
}

/*
*   Used to close database.
*   Should be used after data insertion.
*/
void closeDatabase(void)
{
    sqlite3_close(db);
}
