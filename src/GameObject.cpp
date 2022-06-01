#include "GameObject.h"
#include "Logger.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

GameObject::GameObject(
        std::shared_ptr<Model> model,
        std::shared_ptr<Texture> texture,
        std::unique_ptr<btCollisionShape> collShape,
        btScalar mass,
        const std::string& objectName/*="Object"*/,
        flag_t flags/*=defaultFlags*/
        )
    : m_model{model}
    , m_texture{texture}
    , m_collShape{std::move(collShape)}
    , m_mass{mass}
    , m_name{objectName}
    , m_flags{flags}
    , m_pos{}
    , m_rot{1.0f, 0.0f, 0.0f, 0.0f}
    , m_scale{1.0f, 1.0f, 1.0f}
    , m_modelMatrix{glm::translate(glm::mat4{1.0f}, m_pos)}
{
    Logger::verb << "Created an GameObject (addr=" << this << ", name=" << m_name << ")" << Logger::End;
}

void GameObject::recalcModelMat()
{
    const glm::mat4 trans = glm::translate(glm::mat4{1.0}, m_pos);
    const glm::mat4 rot = glm::mat4_cast(m_rot);
    const glm::mat4 scale = glm::scale(glm::mat4(1.0), m_scale);
    m_modelMatrix = trans * rot * scale;
}

void GameObject::translate(const glm::vec3& vec)
{
    m_pos += vec;
    recalcModelMat();
}

void GameObject::rotate(float angleRad, const glm::vec3& axis)
{
    m_rot = glm::rotate(m_rot, angleRad, axis);
    recalcModelMat();
}

void GameObject::scale(const glm::vec3& scale)
{
    m_scale *= scale;
    recalcModelMat();
}

void GameObject::setPos(const glm::vec3& pos)
{
    m_pos = pos;
    recalcModelMat();
}

void GameObject::setRotationQuat(const glm::quat& quat)
{
    m_rot = quat;
    recalcModelMat();
}

void GameObject::setTextureWrapMode(int horizontalWrapMode, int verticalWrapMode)
{
    m_texture->setWrapMode(horizontalWrapMode, verticalWrapMode);
}

size_t GameObject::draw(unsigned int shaderId)
{
    if ((m_flags & FLAG_VISIBLE) == 0)
        return 0;

    glUniformMatrix4fv(glGetUniformLocation(shaderId, "modelMat"), 1, GL_FALSE, glm::value_ptr(m_modelMatrix));

    m_texture->bind();
    m_model->draw();

    return m_model->getVertCount();
}
