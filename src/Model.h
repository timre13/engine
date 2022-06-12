#pragma once

#include "ui/OverlayRenderer.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#define VERTEX_ATTR_I_VERTEX 0
#define VERTEX_ATTR_I_UV 1
#define VERTEX_ATTR_I_NORMAL 2

class Model final
{
public:
    enum class State
    {
        Uninitialized,
        Ok,
        OpenFailed,
        ParseFailed,
    };

private:
    State m_state{State::Uninitialized};
    size_t m_numOfVertices{};
    float* m_vboData{};
    uint m_vaoIndex{};
    uint m_vboIndex{};

    void _copyVertData(
            const std::vector<float>& verts,
            const std::vector<float>& uvs,
            const std::vector<float>& normals
            );

    friend class GameObject;
    friend class UI::OverlayRenderer;

public:
    Model(){}
    Model(const std::string& path);

    // Copy ctor, copy assignment op
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    // Move ctor, move assignment op
    Model(Model&& another);
    Model& operator=(Model&& another);

    int parseObjFile(
            const std::string& filePath,
            std::vector<float>* outVerts,
            std::vector<float>* outUvs,
            std::vector<float>* outNorms
            );

    int open(const std::string& filePath);
    int fromData(float* values, size_t numOfVertices);

    inline State getState() const { return m_state; }
    inline size_t getVertCount() const { return m_numOfVertices; }

    void draw();

    ~Model();
};

