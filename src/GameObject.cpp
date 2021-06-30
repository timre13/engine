#include "GameObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

GameObject::GameObject(std::shared_ptr<Model> model, std::shared_ptr<Texture> texture, const std::string& objectName/*="Object"*/, flag_t flags/*=defaultFlags*/)
    : m_model{model}, m_texture{texture}, m_name{objectName}, m_flags{flags}
{
    m_modelMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3{0.0f, 0.0f, 0.0f});
}

void GameObject::draw(unsigned int shaderId)
{
    if ((m_flags & FLAG_VISIBLE) == 0)
        return;

    glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMat"), 1, GL_FALSE, glm::value_ptr(m_modelMatrix));

    m_texture->bind();
    m_model->draw();
}
