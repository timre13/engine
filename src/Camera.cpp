#include "Camera.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

Camera::Camera(const glm::vec3& position, float windowAspectRatio)
    : m_position{position}
{
    m_projectionMatrix = glm::perspective(glm::radians(m_fovDeg), windowAspectRatio, 0.1f, 100.0f);
    recalculateFrontVector();
}

void Camera::recalculateFrontVector()
{
    glm::vec3 cameraDir;
    cameraDir.x = cos(glm::radians(m_yawDeg)) * cos(glm::radians(m_pitchDeg));
    cameraDir.y = sin(glm::radians(m_pitchDeg));
    cameraDir.z = sin(glm::radians(m_yawDeg)) * cos(glm::radians(m_pitchDeg));
    m_frontVector = glm::normalize(cameraDir);
}

void Camera::moveForward(float amount)
{
    m_position += m_frontVector * amount;
}

void Camera::moveBackwards(float amount)
{
    m_position -= m_frontVector * amount;
}

void Camera::moveLeft(float amount)
{
    m_position -= glm::normalize(glm::cross(m_frontVector, m_upVector)) * amount;
}

void Camera::moveRight(float amount)
{
    m_position += glm::normalize(glm::cross(m_frontVector, m_upVector)) * amount;
}

void Camera::updateShaderUniforms(uint shaderId)
{
    recalculateFrontVector();
    auto viewMatrix = glm::lookAt(m_position, m_position+m_frontVector, m_upVector);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glUniformMatrix4fv(glGetUniformLocation(shaderId, "projMat"), 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
}

