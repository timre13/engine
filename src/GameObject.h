#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <memory>
#include "Model.h"

class GameObject
{
public:
    using flag_t = uint8_t;
    static constexpr flag_t FLAG_VISIBLE = 1 << 0;
    static constexpr flag_t defaultFlags = FLAG_VISIBLE;

private:
    std::shared_ptr<Model> m_model;
    std::string m_name;
    flag_t m_flags{};

public:
    GameObject(std::shared_ptr<Model>, const std::string& objectName="<Object>", flag_t flags=defaultFlags);
    // Copy ctor, copy assignment op
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    // Move ctor, move assignment op
    GameObject(GameObject&&) = delete;
    GameObject& operator=(GameObject&&) = delete;

    inline flag_t getFlags() const { return m_flags; }
    inline flag_t& getFlags() { return m_flags; }
};

