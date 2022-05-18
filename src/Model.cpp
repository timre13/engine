#include "Model.h"
#include "Logger.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cctype>

#define MODEL_FILE_PARSER_VERBOSE 0

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


Model::Model(const std::string& path)
{
    open(path);
}

Model::Model(Model&& another)
{
    m_state = another.m_state;

    m_numOfVertices = another.m_numOfVertices;
    another.m_numOfVertices = 0;

    m_vboData = another.m_vboData;
    another.m_vboData = nullptr;

    m_vaoIndex = another.m_vaoIndex;
    another.m_vaoIndex = 0;

    m_vboIndex = another.m_vboIndex;
    another.m_vboIndex = 0;
}

Model& Model::operator=(Model&& another)
{
    if (this != &another)
    {
        m_state = another.m_state;

        m_numOfVertices = another.m_numOfVertices;
        another.m_numOfVertices = 0;

        m_vboData = another.m_vboData;
        another.m_vboData = nullptr;

        m_vaoIndex = another.m_vaoIndex;
        another.m_vaoIndex = 0;

        m_vboIndex = another.m_vboIndex;
        another.m_vboIndex = 0;
    }

    return *this;
}

int Model::_parseObjFile(const std::string& filePath)
{
    std::string fileContents;
    if (readFileContents(filePath, &fileContents))
    {
        m_state = State::OpenFailed;
        return 1;
    }

    std::vector<float> verticesTmp;
    std::vector<float> uvCoordsTmp;
    std::vector<float> normalsTmp;
    std::vector<float> vertices;
    std::vector<float> uvCoords;
    std::vector<float> normals;

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
                while (i < fileContents.size() && !std::isspace((unsigned char)fileContents[i]))
                    token += fileContents[i++];
    
                // Skip whitespace
                while (i < fileContents.size() && (fileContents[i] == ' ' || fileContents[i] == '\t'))
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

            verticesTmp.push_back(vertexX);
            verticesTmp.push_back(vertexY);
            verticesTmp.push_back(vertexZ);
        }
        else if (token.compare("vt") == 0)
        {
            getToken();
            float textureX = tokenToFloat();
            getToken();
            float textureY = tokenToFloat();

#if MODEL_FILE_PARSER_VERBOSE
            Logger::verb << "TexCoord(" << textureX << ", " << textureY << ")" << Logger::End;
#endif

            uvCoordsTmp.push_back(textureX);
            uvCoordsTmp.push_back(textureY);
        }
        else if (token.compare("vn") == 0)
        {
            getToken();
            float normalX = tokenToFloat();
            getToken();
            float normalY = tokenToFloat();
            getToken();
            float normalZ = tokenToFloat();

#if MODEL_FILE_PARSER_VERBOSE
            Logger::verb << "Normal(" << normalX << ", " << normalY << ", " << normalZ << ")" << Logger::End;
#endif

            normalsTmp.push_back(normalX);
            normalsTmp.push_back(normalY);
            normalsTmp.push_back(normalZ);
        }
        else if (token.compare("f") == 0)
        {
            for (int i{}; i < 3; ++i)
            {
                getToken();
                std::string originalToken = token;
                size_t vertexI, uvCoordI, normalI;
                size_t numOfProcessed;

                vertexI = std::stoul(token.c_str(), &numOfProcessed);

                try
                {
                    token = token.substr(numOfProcessed+1);
                    uvCoordI = std::stoul(token.c_str(), &numOfProcessed);
                }
                catch (...)
                {
                    Logger::verb << token << Logger::End;
                    Logger::err << "Invalid face specifier (UV coordinates missing? Make sure to export model with UV coordinates included)" << Logger::End;
                    m_state = State::ParseFailed;
                    return 1;
                }

                try
                {
                    token = token.substr(numOfProcessed+1);
                    normalI = std::stoul(token.c_str(), &numOfProcessed);
                }
                catch (...)
                {
                    Logger::err << "Invalid face specifier (Normals missing? Make sure to export model with normals included)" << Logger::End;
                    m_state = State::ParseFailed;
                    return 1;
                }

                if (vertexI  < 1 || vertexI  > verticesTmp.size()
                 || uvCoordI < 1 || uvCoordI > uvCoordsTmp.size()
                 || normalI  < 1 || normalI  > normalsTmp.size())
                {
                    Logger::err << "Invalid face vertex: " << originalToken
                        << ": FaceVertex(" << vertexI << ", " << uvCoordI << ", " << normalI << ")"  << Logger::End;
                    Logger::log << verticesTmp.size() << ", " << uvCoordsTmp.size() << ", " << normalsTmp.size() << Logger::End;
                    m_state = State::ParseFailed;
                    return 1;
                }

#if MODEL_FILE_PARSER_VERBOSE
                Logger::verb << "FaceVertex(" << vertexI << ", " << uvCoordI << ", " << normalI << ")" << Logger::End;
#endif

                vertices.push_back(verticesTmp[(vertexI-1)*3+0]);
                vertices.push_back(verticesTmp[(vertexI-1)*3+1]);
                vertices.push_back(verticesTmp[(vertexI-1)*3+2]);
                uvCoords.push_back(uvCoordsTmp[(uvCoordI-1)*2+0]);
                uvCoords.push_back(uvCoordsTmp[(uvCoordI-1)*2+1]);
                normals.push_back(normalsTmp[(normalI-1)*3+0]);
                normals.push_back(normalsTmp[(normalI-1)*3+1]);
                normals.push_back(normalsTmp[(normalI-1)*3+2]);
            }
            getToken();
            if (token.size())
            {
                Logger::err << "Face element with 3+ values. Re-export model with \"Triangulate faces\" option" << Logger::End;
                m_state = State::ParseFailed;
                return 1;
            }
        }
        else
        {
#if MODEL_FILE_PARSER_VERBOSE
            Logger::verb << "Skipping unsupported keyword: " << token << Logger::End;
#endif
        }

        // Skip the remaining characters in the line
        while (i < fileContents.size() && fileContents[i] != '\n')
            ++i;
        ++i;
    }

    Logger::verb << "Parsed a model with "
        << vertices.size()/3 << " vertices, "
        << uvCoords.size()/2 << " UV coordinates and "
        << normals.size()/3 << " normals"
        << Logger::End;

#if MODEL_FILE_PARSER_VERBOSE
    Logger::verb << "Vertices: ";
    for (float v : vertices)
        Logger::verb << v << ", ";
    Logger::verb << Logger::End;

    Logger::verb << "UVs: ";
    for (float v : uvCoords)
        Logger::verb << v << ", ";
    Logger::verb << Logger::End;

    Logger::verb << "Normals: ";
    for (float v : normals)
        Logger::verb << v << ", ";
    Logger::verb << Logger::End;
#endif

    m_numOfVertices = vertices.size()/3;
    m_vboData = new float[m_numOfVertices*8];
    for (size_t i{}; i < vertices.size()/3; ++i)
    {
        /*
         * Vertex data layout:
         *  * vertex (3 values)
         *  * UV coordinates (2 values)
         *  * normals (3 values)
         */

        const size_t arrayI = i*8;
        m_vboData[arrayI+0] = vertices[i*3+0];
        m_vboData[arrayI+1] = vertices[i*3+1];
        m_vboData[arrayI+2] = vertices[i*3+2];
        m_vboData[arrayI+3] = uvCoords[i*2+0];
        m_vboData[arrayI+4] = uvCoords[i*2+1];
        m_vboData[arrayI+5] = normals[i+0];
        m_vboData[arrayI+6] = normals[i+1];
        m_vboData[arrayI+7] = normals[i+2];
    }
    return 0;
}

static void configureVertexData(uint* vaoIndexPtr, uint* vboIndexPtr, size_t numOfVertices, float* vboData)
{
    assert(vaoIndexPtr);
    assert(vboIndexPtr);
    assert(vboData);

#if MODEL_FILE_PARSER_VERBOSE
    Logger::verb << "VBO data: ";
    for (size_t i{}; i < numOfVertices*8; ++i)
        Logger::verb << vboData[i] << ", ";
    Logger::verb << Logger::End;
#endif

    Logger::verb << "Copying and configuring vertex data" << Logger::End;

    glGenVertexArrays(1, vaoIndexPtr);
    glBindVertexArray(*vaoIndexPtr);

    glGenBuffers(1, vboIndexPtr);
    glBindBuffer(GL_ARRAY_BUFFER, *vboIndexPtr);
    glBufferData(GL_ARRAY_BUFFER, numOfVertices*sizeof(float)*8, vboData, GL_STATIC_DRAW);

    // Vertex coordinate attribute
    glVertexAttribPointer(VERTEX_ATTR_I_VERTEX, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(VERTEX_ATTR_I_VERTEX);

    // Texture coordinate attribute
    glVertexAttribPointer(VERTEX_ATTR_I_UV, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(VERTEX_ATTR_I_UV);

    // Normal attribute
    glVertexAttribPointer(VERTEX_ATTR_I_NORMAL, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(VERTEX_ATTR_I_NORMAL);

    glBindVertexArray(0);
}

int Model::open(const std::string& filePath)
{
    if (_parseObjFile(filePath))
        return 1;

    configureVertexData(&m_vaoIndex, &m_vboIndex, m_numOfVertices, m_vboData);

    m_state = State::Ok;
    Logger::verb << "Model loaded successfully" << Logger::End;
    return 0;
}

int Model::fromData(float* values, size_t numOfVertices)
{
    m_vboData = new float[numOfVertices*8];
    memcpy(m_vboData, values, numOfVertices*8);
    m_numOfVertices = numOfVertices;

    configureVertexData(&m_vaoIndex, &m_vboIndex, m_numOfVertices, m_vboData);

    m_state = State::Ok;
    Logger::verb << "Model loaded successfully" << Logger::End;
    return 0;
}

void Model::draw()
{
    glBindVertexArray(m_vaoIndex);
    glDrawArrays(GL_TRIANGLES, 0, m_numOfVertices);
}

Model::~Model()
{
    glDeleteVertexArrays(1, &m_vaoIndex);
    glDeleteBuffers(1, &m_vboIndex);
    delete[] m_vboData;
    Logger::verb << "Deleted a model (" << this << ')' << Logger::End;
}

