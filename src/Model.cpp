#include "Model.h"
#include "Logger.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include <cctype>

#define MODEL_FILE_PARSER_VERBOSE 1

static bool readFileContents(const std::string& filePath, std::string* output)
{
    Logger::verb << "Reading model file: " << filePath << Logger::End;

    std::ifstream fileObject;
    try
    {
        fileObject.open(filePath);
        if (!fileObject.is_open())
        {
            Logger::err << "Failed to open file: " << filePath << ": " << strerror(errno) << Logger::End;
            return 1;
        }
        std::stringstream sstream;
        sstream << fileObject.rdbuf();
        *output = sstream.str();
    }
    catch (std::exception& e)
    {
        Logger::err << "Failed to read file: " << filePath << ": " << e.what() << Logger::End;
        fileObject.close();
        return 1;
    }

    fileObject.close();
    return 0;
}

bool Model::open(const std::string& filePath)
{
    std::string fileContents;
    if (readFileContents(filePath, &fileContents))
    {
        return 1;
    }

    for (size_t i{}; i < fileContents.size();)
    {
        std::string token;

        // Skip comments
        if (fileContents[i] == '#')
        {
            while (i < fileContents.size() && fileContents[i] != '\n')
                ++i;
        }

        auto getToken{
            [&](){
                token.clear();

                // Get token
                while (i < fileContents.size() && !std::isspace(fileContents[i]))
                    token += fileContents[i++];
    
                // Skip whitespace
                while (i < fileContents.size() && std::isspace(fileContents[i]))
                    ++i;
            }
        };

        auto tokenToFloat{
            [&](){
                try
                {
                    return std::stof(token.c_str(), nullptr);
                }
                catch (...)
                {
                    return 0.0f;
                }
            }
        };

        getToken();

        if (token.compare("v") == 0)
        {
            getToken();
            float vertexX = tokenToFloat();
            getToken();
            float vertexY = tokenToFloat();
            getToken();
            float vertexZ = tokenToFloat();

#if MODEL_FILE_PARSER_VERBOSE
            Logger::verb << "Vertex(" << vertexX << ", " << vertexY << ", " << vertexZ << ")" << Logger::End;
#endif
            m_vertices.push_back(vertexX);
            m_vertices.push_back(vertexY);
            m_vertices.push_back(vertexZ);
        }
        else // TODO: Implement "vt"
        {
#if MODEL_FILE_PARSER_VERBOSE
            Logger::verb << "Skipping: " << token << Logger::End;
#endif
            continue;
        }

    }

    return 0;
}

