#include "Widget.h"
#include "../Logger.h"

namespace UI
{

bool Widget::isInside(const glm::vec2& position) const
{
    return (position.x >= m_position.x && position.x < m_position.x + m_size.x &&
            position.y >= m_position.y && position.y < m_position.y + m_size.y);
}

void Widget::onCursorMove(float mouseX, float mouseY)
{
    if (m_mouseMoveCb && isInside({mouseX, mouseY}))
        m_mouseMoveCb(this, mouseX, mouseY);
}

void Widget::onMouseClick(float clickX, float clickY)
{
    if (m_mouseClickCb && isInside({clickX, clickY}))
        m_mouseClickCb(this, clickX, clickY);
}

};

