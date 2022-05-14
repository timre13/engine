#include "GameObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

GameObject::GameObject(std::shared_ptr<Model> model, std::shared_ptr<Texture> texture, const std::string& objectName/*="Object"*/, flag_t flags/*=defaultFlags*/)
    : m_model{model}, m_texture{texture}, m_name{objectName}, m_flags{flags}
{
    m_pos = {0.0f, 0.0f, 0.0f};
    m_modelMatrix = glm::translate(glm::mat4{1.0f}, m_pos);
}

void GameObject::translate(const glm::vec3& vec)
{
    m_modelMatrix = glm::translate(m_modelMatrix, vec);
    m_pos += vec;
}

void GameObject::rotate(float angleRad, const glm::vec3& axis)
{
    m_modelMatrix = glm::rotate(m_modelMatrix, angleRad, axis);
}

void GameObject::scale(const glm::vec3& scale)
{
    m_modelMatrix = glm::scale(m_modelMatrix, scale);
}

void GameObject::setPos(const glm::vec3& pos)
{
    m_modelMatrix = glm::translate(m_modelMatrix, pos-m_pos);
    m_pos = pos;
}

void GameObject::setTextureWrapMode(int horizontalWrapMode, int verticalWrapMode)
{
    m_texture->setWrapMode(horizontalWrapMode, verticalWrapMode);
}

void GameObject::draw(unsigned int shaderId)
{
    if ((m_flags & FLAG_VISIBLE) == 0)
        return;

    glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMat"), 1, GL_FALSE, glm::value_ptr(m_modelMatrix));

    m_texture->bind();
    m_model->draw();
}
