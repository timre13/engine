#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <bullet/LinearMath/btIDebugDraw.h>
#include "Logger.h"
#include "ShaderProgram.h"
#include "Camera.h"

class PhysicsDebugDraw final : public btIDebugDraw
{
private:
    uint            m_lineVAO{};
    uint            m_lineVBO{};
    ShaderProgram   m_lineShader;
    float           m_lineVerts[3*2]{};

    DebugDrawModes  m_drawMode{};

public:
    PhysicsDebugDraw()
    {
        m_lineShader.open("../shaders/line.vert.glsl", "../shaders/line.frag.glsl");

        glGenVertexArrays(1, &m_lineVAO);
        glBindVertexArray(m_lineVAO);

        glGenBuffers(1, &m_lineVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_lineVerts), nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
        glEnableVertexAttribArray(0);
    }

    void updateUniforms(Camera& cam)
    {
        m_lineShader.use();
        cam.updateShaderUniforms(m_lineShader.getId());
    }

    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        (void)color; // TODO: Set color in shader

        glBindVertexArray(m_lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);

        m_lineVerts[0] = from.x();
        m_lineVerts[1] = from.y();
        m_lineVerts[2] = from.z();
        m_lineVerts[3] = to.x();
        m_lineVerts[4] = to.y();
        m_lineVerts[5] = to.z();

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_lineVerts), m_lineVerts);
        glDrawArrays(GL_LINES, 0, 2);
    }

    virtual void drawContactPoint(
            const btVector3& PointOnB,
            const btVector3& normalOnB,
            btScalar distance,
            int lifeTime,
            const btVector3& color)
    {
        // TODO
    }

    virtual void reportErrorWarning(const char* warningString)
    {
        // TODO
    }

    virtual void draw3dText(const btVector3& location, const char* textString)
    {
        // TODO
    }

    virtual inline void setDebugMode(int debugMode)
    {
        m_drawMode = (btIDebugDraw::DebugDrawModes)debugMode;
    }

    virtual inline int getDebugMode() const
    {
        return m_drawMode;
    }

    ~PhysicsDebugDraw()
    {
        glDeleteVertexArrays(1, &m_lineVAO);
        glDeleteBuffers(1, &m_lineVBO);
    }
};
