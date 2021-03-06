#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bullet/LinearMath/btVector3.h>

class Camera
{
private:
    glm::vec3 m_position;
    float m_fovDeg{45.0f};
    glm::mat4 m_projectionMatrix;
    float m_yawDeg{-90.0f};
    float m_pitchDeg{};
    glm::vec3 m_frontVector{};
    glm::vec3 m_upVector{0.0f, 1.0f, 0.0f};
    float m_windowAspectRatio{};

public:
    Camera(const glm::vec3& position, float windowAspectRatio);

    inline void setFovDeg(float valueDeg) { m_fovDeg = valueDeg; setWindowAspectRatio(m_windowAspectRatio); }
    inline float getFovDeg() const { return m_fovDeg; }

    inline void moveForward(float amount, int frameTime) { m_position += m_frontVector * (amount * frameTime); }
    inline void moveBackwards(float amount, int frameTime) { m_position -= m_frontVector * (amount * frameTime); }
    inline void moveLeft(float amount, int frameTime) { m_position -= glm::normalize(glm::cross(m_frontVector, m_upVector)) * (amount * frameTime); }
    inline void moveRight(float amount, int frameTime) { m_position += glm::normalize(glm::cross(m_frontVector, m_upVector)) * (amount * frameTime); }

    void recalculateFrontVector();
    inline void setWindowAspectRatio(float rat)
    {
        m_windowAspectRatio = rat;
        m_projectionMatrix = glm::perspective(glm::radians(m_fovDeg), m_windowAspectRatio, 0.01f, 1000.0f);
    }

    void updateShaderUniforms(uint shaderId);

    inline void setYawDeg(float val) { m_yawDeg = val; }
    inline float getYawDeg() { return m_yawDeg; }

    inline void setPitchDeg(float val)
    {
        m_pitchDeg = val;
        if (m_pitchDeg > 89.9)
            m_pitchDeg = 89.9;
        if (m_pitchDeg < -89.9)
            m_pitchDeg = -89.9;
    }
    inline float getPitchDeg() { return m_pitchDeg; }

    inline const glm::vec3& getPosition() const { return m_position; }
    btVector3 getPositionBt() const;
    btVector3 getFrontPosBt(float frontVecLen=1.0f) const;
    btVector3 getFrontVecBt() const;
};

