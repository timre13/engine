#include "Button.h"
#include "Window.h"

namespace UI
{

void Button::draw()
{
    auto parent = getTopLevelParent();
    assert(parent);
    auto window = dynamic_cast<Window*>(parent);
    assert(window);
    auto renderer = window->getRenderer();
    renderer->drawFilledRectangle(m_position, m_size, m_isHovered ? m_hoveredBgColor : m_bgColor);
    renderer->renderTextAtPerc(
            m_text,
            1.0f,
            {m_position.x+m_size.x/2-(float)DEF_FONT_SIZE/renderer->getWindowWidth()*25*m_text.size(),
            m_position.y+m_size.y/2-(float)DEF_FONT_SIZE/renderer->getWindowHeight()*25},
            m_textColor);
}

void Button::onCursorMove(float mouseX, float mouseY)
{
    if (isInside({mouseX, mouseY}))
    {
        if (m_mouseMoveCb)
            m_mouseMoveCb(this, mouseX, mouseY);

        m_isHovered = true;
    }
    else
    {
        m_isHovered = false;
    }
}

}

