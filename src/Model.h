#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class Model
{
private:
    std::vector<float> m_vertices;
    std::vector<float> m_textureCoords;

public:
    Model(){}

    bool open(const std::string& filePath);
};

