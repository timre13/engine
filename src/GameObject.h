#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/ext/matrix_transform.hpp>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Model.h"
#include "Texture.h"

class GameObject
{
public:
    using flag_t = uint8_t;
    static constexpr flag_t FLAG_VISIBLE = 1 << 0;
    static constexpr flag_t defaultFlags = FLAG_VISIBLE;

protected:
    std::shared_ptr<Model> m_model;
    std::shared_ptr<Texture> m_texture;
    std::string m_name;
    flag_t m_flags{};
    glm::mat4 m_modelMatrix;

public:
    GameObject(std::shared_ptr<Model> model, std::shared_ptr<Texture> texture, const std::string& objectName="<Object>", flag_t flags=defaultFlags);
    // Copy ctor, copy assignment op
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    // Move ctor, move assignment op
    GameObject(GameObject&&) = delete;
    GameObject& operator=(GameObject&&) = delete;

    inline flag_t getFlags() const { return m_flags; }
    inline flag_t& getFlags() { return m_flags; }

    inline void translate(const glm::vec3 vec) { m_modelMatrix = glm::translate(m_modelMatrix, vec); }
    inline void rotate(float angleRad, const glm::vec3 axis) { m_modelMatrix = glm::rotate(m_modelMatrix, angleRad, axis); }
    inline void scale(const glm::vec3& scale) { m_modelMatrix = glm::scale(m_modelMatrix, scale); }

    void draw(unsigned int shaderId);

    inline void setTextureWrapMode(int horizontalWrapMode, int verticalWrapMode) { m_texture->setWrapMode(horizontalWrapMode, verticalWrapMode); }
};

