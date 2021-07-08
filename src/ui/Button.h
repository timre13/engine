#pragma once

#include "Widget.h"
#include <string>
#include <glm/glm.hpp>

namespace UI
{

class Button : public Widget
{
protected:
    bool m_isHovered{};
    glm::vec3 m_hoveredBgColor;

public:
    virtual inline bool isHovered() const { return m_isHovered; }
    virtual inline void setHoveredBgColor(const glm::vec3& value) { m_hoveredBgColor = value; }
    virtual inline const glm::vec3& getHoveredBgColor() const { return m_hoveredBgColor; }

    virtual void draw() override;

    virtual void onCursorMove(float mouseX, float mouseY) override;
};

}

