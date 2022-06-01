#include "Camera.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

Camera::Camera(const glm::vec3& position, float windowAspectRatio)
    : m_position{position}
{
    setWindowAspectRatio(windowAspectRatio);
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

void Camera::updateShaderUniforms(uint shaderId)
{
    recalculateFrontVector();
    auto viewMatrix = glm::lookAt(m_position, m_position+m_frontVector, m_upVector);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glUniformMatrix4fv(glGetUniformLocation(shaderId, "projMat"), 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
}

btVector3 Camera::getPositionBt() const
{
    return {m_position.x, m_position.y, m_position.z};
}

btVector3 Camera::getFrontPosBt(float frontVecLen/*=1.0f*/) const
{
    const glm::vec3 front = m_position + m_frontVector*frontVecLen;
    return {front.x, front.y, front.z};
}

btVector3 Camera::getFrontVecBt() const
{
    return {m_frontVector.x, m_frontVector.y, m_frontVector.z};
}
