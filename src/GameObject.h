#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <bullet/BulletCollision/btBulletCollisionCommon.h>
#include "Model.h"
#include "Texture.h"

class GameObject
{
public:
    using flag_t = uint8_t;
    static constexpr flag_t FLAG_VISIBLE = 1 << 0;
    static constexpr flag_t defaultFlags = FLAG_VISIBLE;

    static constexpr btScalar MASS_STATIC = 0.0f;

protected:
    std::shared_ptr<Model> m_model;
    glm::quat m_mRot; // Model rotation
    std::shared_ptr<Texture> m_texture;

    std::unique_ptr<btCollisionShape> m_collShape{};
    btScalar m_mass{};

    std::string m_name;
    flag_t m_flags{};

    glm::vec3 m_pos;
    glm::quat m_rot;
    glm::vec3 m_scale;
    glm::mat4 m_modelMatrix;

    void recalcModelMat();

    friend class PhysicsWorld;

public:
    GameObject(
            std::shared_ptr<Model> model,
            const glm::vec3& modelRotRad,
            std::shared_ptr<Texture> texture,
            std::unique_ptr<btCollisionShape> collShape,
            btScalar mass,
            const std::string& objectName="<Object>",
            flag_t flags=defaultFlags);
    // Copy ctor, copy assignment op
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    // Move ctor, move assignment op
    GameObject(GameObject&&) = delete;
    GameObject& operator=(GameObject&&) = delete;

    inline flag_t getFlags() const { return m_flags; }
    inline flag_t& getFlags() { return m_flags; }
    inline const std::string& getName() const { return m_name; }

    void translate(const glm::vec3& vec);
    void rotate(float angleRad, const glm::vec3& axis);
    void scale(const glm::vec3& scale);

    void setPos(const glm::vec3& pos);
    void setRotationQuat(const glm::quat& quat);

    void setTextureWrapMode(int horizontalWrapMode, int verticalWrapMode);

    // Returns: The number of vertices drawn
    size_t draw(unsigned int shaderId);
};

