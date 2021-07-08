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
    window->getRenderer()->drawFilledRectangle(m_position, m_size, m_bgColor);
}

}

