#pragma once

#include "util/Common.hpp"
#include "util/entity_parser.hpp"

#define DEFAULT_SIGNATURE_DATABASE_PATH "/etc/scl/ConfigService/tmp/signature.txt"

class FileSecurity
{
private:
    vector<SignatureEntity> readSignaturesFromFile()
    {
        vector<SignatureEntity> entity;
        std::ifstream file(DEFAULT_SIGNATURE_DATABASE_PATH);
        string line;

        if (file.is_open())
        {
            while (std::getline(file, line))
            {
                std::istringstream iss(line);
                string file_name, signature;
                if (iss >> file_name >> signature)
                {
                    entity.push_back({file_name, signature});
                }
            }
            file.close();
        }

        return entity;
    }

    int writeSignatureToFile(const SignatureEntity &entity)
    {
        std::ofstream file(DEFAULT_SIGNATURE_DATABASE_PATH, std::ios::binary);
        if (file.is_open())
        {
            file << entity.file_name << " " << entity.signature << '\n';
            file.close();
            DEBUG(entity.file_name + " success.");
            return SUCCESS;
        }
        else
        {
           LOG_ERROR("failed to write to " + entity.file_name, FAILED);
            return FAILED;
        }
    }

    void writeSignatureToFile(const vector<SignatureEntity> &entities)
    {
        for (const SignatureEntity &entity : entities)
        {
            writeSignatureToFile(entity);
        }
    }

public:
    FileSecurity()
    {
        os::createFile(DEFAULT_SIGNATURE_DATABASE_PATH);
    }

    int signAndStoreSignature(const string &file, const string &sign_key)
    {
        string signature_data = os::sign(file, sign_key);

        if (signature_data.empty())
        {
            LOG_ERROR("signed data is empty");
            return FAILED;
        }

        SignatureEntity entity(file, signature_data);

        return (writeSignatureToFile(entity) == FAILED) ? FAILED : SUCCESS;
    }

    bool verifySignature(const string &file_name, const string &sign_key)
    {
        bool result = true;
        vector<SignatureEntity> entities = readSignaturesFromFile();

        auto it = std::remove_if(entities.begin(), entities.end(), [&file_name, &sign_key, &result](const SignatureEntity &entity)
                                 {
            string new_signature = os::sign(file_name, sign_key);
            std::cout << entity.file_name << " " << file_name << '\n' << "old size: " << entity.signature.length() << '\t' << "new size: " << new_signature.length();
            if (entity.file_name == file_name &&  new_signature == entity.signature)
            {
                DEBUG("signature verification successfull. removing entry.");
                return true;
            }
            return false; });

        std::cout << "before erase size : " << entities.size() << '\n';
        entities.erase(it, entities.end());

        std::cout << "after erase size : " << entities.size() << '\n';

        if (!entities.empty())
        {
            writeSignatureToFile(entities);
        }
        else 
        {
            std::ofstream file(DEFAULT_SIGNATURE_DATABASE_PATH, std::ios::trunc);
            file.close();
        }

        return result;
    }
};