#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace UI
{

class Widget
{
protected:
    glm::vec2 m_position{};
    glm::vec2 m_size{};
    glm::vec3 m_bgColor{};
    glm::vec3 m_borderColor{};
    Widget* m_parent{};

    friend class Container;
    inline void setParent(Widget* parent) { m_parent = parent; }

public:
    Widget() = default;
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget(Widget&&) = delete;
    Widget& operator=(Widget&&) = delete;

    inline Widget* getParent() { return m_parent; }
    inline Widget* getTopLevelParent()
    {
        auto parent = m_parent;
        while (true)
        {
            if (!parent)
                break;

            parent = parent->getParent();
        }

        return parent;
    }

    virtual inline void setPos(const glm::vec2& value) { m_position = value; }
    virtual inline const glm::vec2& getPos() const { return m_position; }
    virtual inline void setXPos(float value) { m_position.x = value; }
    virtual inline float getXPos() const { return m_position.x; }
    virtual inline void setYPos(float value) { m_position.y = value; }
    virtual inline float getYPos() const { return m_position.y; }

    virtual inline void setSize(const glm::vec2& value) { m_size = value; }
    virtual inline const glm::vec2& getSize() const { return m_size; }
    virtual inline void setWidth(float value) { m_size.x = value; }
    virtual inline float getWidth() const { return m_size.x; }
    virtual inline void setHeight(float value) { m_size.y = value; }
    virtual inline float getHeight() const { return m_size.y; }

    virtual inline void setBgColor(const glm::vec3& value) { m_bgColor = value; }
    virtual inline const glm::vec3& getBgColor() { return m_bgColor; }

    virtual inline void setBorderColor(const glm::vec3& value) { m_borderColor = value; }
    virtual inline const glm::vec3& getBorderColor() { return m_borderColor; }

    virtual void draw() = 0;

    virtual inline ~Widget() {}
};

}

