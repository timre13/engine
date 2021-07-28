#pragma once

#include "Widget.h"
#include "colors.h"
#include <string>
#include <glm/glm.hpp>

namespace UI
{

class Button : public Widget
{
protected:
    std::string m_text;
    glm::vec3 m_textColor = UI_COLOR_TEXT;

    bool m_isHovered{};
    glm::vec3 m_hoveredBgColor = UI_COLOR_WIDG_BG_HOVERED;

public:
    virtual inline void setText(const std::string& text) { m_text = text; }
    virtual inline const std::string& getText() const { return m_text; }
    virtual inline void setTextColor(const glm::vec3& value) { m_textColor = value; }
    virtual inline const glm::vec3& getTextColor() const { return m_textColor; }

    virtual inline bool isHovered() const { return m_isHovered; }
    virtual inline void setHoveredBgColor(const glm::vec3& value) { m_hoveredBgColor = value; }
    virtual inline const glm::vec3& getHoveredBgColor() const { return m_hoveredBgColor; }

    virtual void draw() override;

    virtual void onCursorMove(float mouseX, float mouseY) override;
};

}

