#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

public:
    Camera(const glm::vec3& position, float windowAspectRatio);

    inline void moveForward(float amount, int frameTime) { m_position += m_frontVector * (amount * frameTime); }
    inline void moveBackwards(float amount, int frameTime) { m_position -= m_frontVector * (amount * frameTime); }
    inline void moveLeft(float amount, int frameTime) { m_position -= glm::normalize(glm::cross(m_frontVector, m_upVector)) * (amount * frameTime); }
    inline void moveRight(float amount, int frameTime) { m_position += glm::normalize(glm::cross(m_frontVector, m_upVector)) * (amount * frameTime); }

    void recalculateFrontVector();
    inline void setWindowAspectRatio(float rat) { m_projectionMatrix = glm::perspective(glm::radians(m_fovDeg), rat, 0.1f, 100.0f); }

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
};

