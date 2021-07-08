#include "Window.h"

namespace UI
{

Window::Window(std::shared_ptr<OverlayRenderer> renderer)
    : m_renderer{renderer}
{
}

void Window::draw()
{
    m_renderer->drawFilledRectangle(m_position, m_size, m_bgColor);

    for (auto& child : m_children)
        child->draw();
}

}

