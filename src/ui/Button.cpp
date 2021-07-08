#include "Button.h"
#include "Window.h"

namespace UI
{

void Button::draw()
{
    auto parent = getParent();
    assert(parent);
    auto window = dynamic_cast<Window*>(parent);
    assert(window);
    window->getRenderer()->drawFilledRectangle(m_position, m_size, m_isHovered ? m_hoveredBgColor : m_bgColor);
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

