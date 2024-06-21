#ifndef DATABASE_SERVICE_HPP
#define DATABASE_SERVICE_HPP
#pragma once

#include "util/Common.hpp"
#include "tpm/TpmEntity.hpp"

using namespace SCM::Tpm;

class DbService
{
public:
    DbService(const string &database) : dbPath(database)
    {
        init(database);
    }

    ~DbService()
    {
        if (connection)
        {
            sqlite3_close(connection);
        }
    }

    void init(const string &dbPath)
    {
        (void)getConnection(dbPath);
    }

    int getConnection(const string &dbPath)
    {
        if (dbPath.empty())
        {
            LOG_ERROR("TPM2_INVALID_DATABASE");
            return TPM2_INVALID_DATABASE;
        }

        int flags = SQLITE_OPEN_READWRITE; //| SQLITE_OPEN_CREATE;

        if (sqlite3_open_v2(dbPath.c_str(), &connection, flags, nullptr) != SQLITE_OK)
        {
            LOG_ERROR("Failed to open the database.");
            sqlite3_close(connection);
            return TPM2_INVALID_DATABASE;
        }

        else if (sqlite3_errcode(connection) == SQLITE_MISUSE && sqlite3_open_v2(dbPath.c_str(), &connection, flags, nullptr) != SQLITE_OK)
        {
            LOG_ERROR("Failed to open the database.");
            sqlite3_close(connection);
            return TPM2_INVALID_DATABASE;
        }
        return SCM::SUCCESS;
    }

    int addTpmContext(const TpmContext &handle)
    {
        int handleValid = validateTpmContext(handle);
        if (handleValid != TPM2_SUCCESS)
        {
            return TPM2_INVALID_CONTEXT;
        }
        sqlite3_stmt *stmt = nullptr;
        TPM2_RC response = 0;
        const char *QUERY = "INSERT INTO handles (handle_name, handle_password, handle_index, data_size) VALUES (?,?,?,?);";
        int success = 0;
        int state = getConnection(dbPath);
        try
        {
            if (connection && state == SCM::SUCCESS)
            {
                if (sqlite3_prepare_v2(connection, QUERY, strlen(QUERY), &stmt, nullptr) == SQLITE_OK)
                {
                    sqlite3_bind_text(stmt, 1, handle.keyName.c_str(), strlen(handle.keyName.c_str()), SQLITE_STATIC);
                    sqlite3_bind_text(stmt, 2, handle.dekAuth.c_str(), strlen(handle.dekAuth.c_str()), SQLITE_STATIC);
                    sqlite3_bind_int(stmt, 3, handle.index);
                    sqlite3_bind_int(stmt, 4, handle.dataSize);
                    success = sqlite3_step(stmt);
                    sqlite3_finalize(stmt);
                    if (success == SQLITE_DONE)
                    {
                        response = TPM2_SUCCESS;
                        INFO("TpmContext Created Successfully");
                    }
                    else
                    {
                        throw std::invalid_argument("TpmContext creation failed.");
                    }
                }
                else
                {
                    throw std::invalid_argument("Prepare statement error");
                }
            }
            else
            {
                throw std::invalid_argument("Connection not established");
            }
            sqlite3_close(connection);
        }
        catch (std::exception &ex)
        {
            LOG_ERROR(ex.what());
            if (connection)
            {
                sqlite3_close(connection);
            }
        }
        return response;
    }

    int deleteHandle(const TpmContext &handle)
    {
        int handleValid = validateTpmContext(handle);
        if (handleValid != TPM2_SUCCESS)
        {
            return TPM2_INVALID_CONTEXT;
        }
        TPM2_RC response = 0;
        sqlite3_stmt *stmt = nullptr;
        const char *QUERY = "DELETE FROM handles WHERE handle_name = ? and handle_password = ?";
        int success = 0;
        int state = getConnection(dbPath);
        try
        {
            if (connection && state == SCM::SUCCESS)
            {
                if (sqlite3_prepare_v2(connection, QUERY, strlen(QUERY), &stmt, nullptr) == SQLITE_OK)
                {
                    sqlite3_bind_text(stmt, 1, handle.keyName.c_str(), strlen(handle.keyName.c_str()), SQLITE_STATIC);
                    sqlite3_bind_text(stmt, 2, handle.dekAuth.c_str(), strlen(handle.dekAuth.c_str()), SQLITE_STATIC);
                    success = sqlite3_step(stmt);
                    sqlite3_finalize(stmt);
                    if (success == SQLITE_DONE)
                    {
                        response = TPM2_SUCCESS;
                        DEBUG("TpmContext Handle Deleted Successfully");
                    }
                    else
                    {
                        throw std::invalid_argument("TpmContext Delete Handle Failed\n");
                    }
                }
                else
                {
                    throw std::invalid_argument("Prepare statement error");
                }
            }
            else
            {
                throw std::invalid_argument("Connection not established\n");
            }
            sqlite3_close(connection);
        }
        catch (std::exception &ex)
        {
            LOG_ERROR(ex.what());
            if (connection)
            {
                sqlite3_close(connection);
            }
        }
        return response;
    }

    int getTpmContextByKeyNameAndDekAuth(TpmContext &handle)
    {
        TPM2_RC response = 0;
        sqlite3_stmt *stmt = nullptr;
        const char *QUERY = "SELECT * FROM handles WHERE handle_name = ? and handle_password = ?";
        int success = 0;
        int state = getConnection(dbPath);
        try
        {
            if (connection && state == SCM::SUCCESS)
            {
                if (sqlite3_prepare_v2(connection, QUERY, strlen(QUERY), &stmt, nullptr) == SQLITE_OK)
                {
                    sqlite3_bind_text(stmt, 1, handle.keyName.c_str(), strlen(handle.keyName.c_str()), SQLITE_STATIC);
                    sqlite3_bind_text(stmt, 2, handle.dekAuth.c_str(), strlen(handle.dekAuth.c_str()), SQLITE_STATIC);
                    success = sqlite3_step(stmt);

                    if (success == SQLITE_ROW)
                    {
                        handle.index = sqlite3_column_int(stmt, 3);
                        handle.dataSize = sqlite3_column_int(stmt, 4);
                        sqlite3_finalize(stmt);
                        response = TPM2_DUPLICATE_VALUE;
                        DEBUG("Index already defined.\n");
                    }
                    else
                    {
                        throw std::invalid_argument("index not exists.\n");
                    }
                }
                else
                {
                    throw std::invalid_argument("Prepare statement error\n");
                }
            }
            else
            {
                throw std::invalid_argument("Connection not established\n");
            }
            sqlite3_close(connection);
        }
        catch (std::exception &ex)
        {
            LOG_ERROR(ex.what());
            if (connection)
            {
                sqlite3_close(connection);
            }
        }
        return response;
    }

    int getTpmContextByIndex(int &index)
    {
        TpmContext handle;
        TPM2_RC response = 0;
        sqlite3_stmt *stmt = nullptr;
        const char *QUERY = "SELECT * FROM handles WHERE handle_index = ?";
        int success = 0;
        int state = getConnection(dbPath);
        try
        {
            if (connection && state == SCM::SUCCESS)
            {
                if (sqlite3_prepare_v2(connection, QUERY, strlen(QUERY), &stmt, nullptr) == SQLITE_OK)
                {
                    sqlite3_bind_int(stmt, 1, handle.index);
                    success = sqlite3_step(stmt);
                    sqlite3_finalize(stmt);
                    if (success == SQLITE_ROW)
                    {
                        response = TPM2_DUPLICATE_VALUE;
                        DEBUG("Index already defined.\n");
                    }
                    else
                    {
                        throw std::invalid_argument("index not exists.\n");
                    }
                }
                else
                {
                    throw std::invalid_argument("Prepare statement error");
                }
            }
            else
            {
                throw std::invalid_argument("Connection not established\n");
            }
            sqlite3_close(connection);
        }
        catch (std::exception &ex)
        {
            LOG_ERROR(ex.what());
            if (connection)
            {
                sqlite3_close(connection);
            }
        }
        return response;
    }

    int getTpmContextByName(const string &keyName)
    {
        TPM2_RC response = 0;
        sqlite3_stmt *stmt = nullptr;
        const char *QUERY = "SELECT * FROM handles WHERE handle_name = ?";
        int success = 0;
        int state = getConnection(dbPath);
        try
        {
            if (connection && state == SCM::SUCCESS)
            {
                if (sqlite3_prepare_v2(connection, QUERY, strlen(QUERY), &stmt, nullptr) == SQLITE_OK)
                {
                    sqlite3_bind_text(stmt, 1, keyName.c_str(), strlen(keyName.c_str()), SQLITE_STATIC);
                    success = sqlite3_step(stmt);
                    sqlite3_finalize(stmt);
                    if (success == SQLITE_ROW)
                    {
                        response = TPM2_DUPLICATE_VALUE;
                        DEBUG("Index already defined.\n");
                    }
                    else
                    {
                        throw std::invalid_argument("index not exists.\n");
                    }
                }
                else
                {
                    throw std::invalid_argument("Prepare statement error");
                }
            }
            else
            {
                response = TPM2_INVALID_DATABASE;
                throw std::invalid_argument("Connection not established");
            }
            sqlite3_close(connection);
        }
        catch (std::exception &ex)
        {
            LOG_ERROR(ex.what());
            if (connection)
            {
                sqlite3_close(connection);
            }
        }
        return response;
    }

    int deleteTpmContextAllHandles()
    {
        TPM2_RC response = 0;
        sqlite3_stmt *stmt = nullptr;
        const char *QUERY = "DELETE FROM handles";
        int success = 0;
        int state = getConnection(dbPath);
        try
        {
            if (connection && state == SCM::SUCCESS)
            {
                if (sqlite3_prepare_v2(connection, QUERY, strlen(QUERY), &stmt, nullptr) == SQLITE_OK)
                {
                    success = sqlite3_step(stmt);
                    if (success == SQLITE_DONE)
                    {
                        response = TPM2_SUCCESS;
                        DEBUG("TpmContext Handles Removed. \n");
                    }
                    else
                    {
                        throw std::invalid_argument("TpmContext Handles not Removed.\n");
                    }
                }
                else
                {
                    throw std::invalid_argument("Prepare statement error");
                }
            }
            else
            {
                throw std::invalid_argument("Connection not established\n");
            }
            sqlite3_close(connection);
        }
        catch (std::exception &ex)
        {
            LOG_ERROR(ex.what());
            if (connection)
            {
                sqlite3_close(connection);
            }
        }
        return response;
    }

private:
    TPM2_RC validateTpmContext(const TpmContext &context)
    {
        if (context.dekAuth.empty())
        {
            DEBUG("TPM2_INVALID_CONTEXT");
            return TPM2_EMPTY_STRING;
        }
        else if (context.keyName.empty())
        {
            DEBUG("TPM2_INVALID_CONTEXT");
            return TPM2_EMPTY_STRING;
        }
        return TPM2_SUCCESS;
    }

private:
    string dbPath;
    sqlite3 *connection;
};

#endif // !DATABASE_SERVICE_HPP