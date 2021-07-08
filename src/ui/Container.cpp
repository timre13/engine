#include "Container.h"

namespace UI
{

void Container::onCursorMove(float mouseX, float mouseY)
{
    if (isInside({mouseX, mouseY}))
    {
        if (m_mouseMoveCb)
            m_mouseMoveCb(this, mouseX, mouseY);

        for (auto& child : m_children)
            child->onCursorMove(mouseX, mouseY);
    }
}

void Container::onMouseClick(float clickX, float clickY)
{
    if (isInside({clickX, clickY}))
    {
        if (m_mouseClickCb)
            m_mouseClickCb(this, clickX, clickY);

        for (auto& child : m_children)
            child->onMouseClick(clickX, clickY);
    }
}

}

