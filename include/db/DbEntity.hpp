#ifndef DATABASE_ENTITY_HPP
#define DATABASE_ENTITY_HPP
 
#include <sqlite3.h>
#include <stdio>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <regex.h>
 
 
#define DATABASE_PATH "secrets"
#define NAME_SIZE 20
#define PASSWORD_SIZE 20
#define USER_ROLE_SIZE 5
#define TIME_SIZE 20
#define DATA_SIZE 100
 
typedef struct USER_CONTEXT USER_CONTEXT;
typedef struct LOGIN_CONTEXT LOGIN_CONTEXT;
typedef struct LOG_CONTEXT LOG_CONTEXT;
typedef struct PERSISTED_CONTEXT PERSISTED_CONTEXT;
 

struct USER_CONTEXT
{
    char userName[NAME_SIZE];
    char userEmail[NAME_SIZE];
    char userPassword[PASSWORD_SIZE];
    char userRole[USER_ROLE_SIZE];
};
 
struct LOGIN_CONTEXT
{
    char userEmail[NAME_SIZE];
    char userPassword[PASSWORD_SIZE];
};
 
struct LOG_CONTEXT
{
    int userId;
    char time[TIME_SIZE];
    char activity[TIME_SIZE];
    char status[DATA_SIZE];
    char ipAddress[NAME_SIZE];
};
 
struct PERSISTED_CONTEXT
{
    char indexName[PASSWORD_SIZE];
    char ownerPassword[PASSWORD_SIZE];
    char data[DATA_SIZE];
    int indexValue;
    char indexPassword[PASSWORD_SIZE];
    int dataSize;
};
 
 
#endif // !DATABASE_ENTITY_HPP