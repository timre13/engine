#pragma once

#include "OverlayRenderer.h"
#include "Container.h"

namespace UI
{

class Window final : public Container
{
protected:
    std::shared_ptr<OverlayRenderer> m_renderer;

public:
    Window(std::shared_ptr<OverlayRenderer> renderer);

    virtual void draw() override;
    virtual inline std::shared_ptr<OverlayRenderer> getRenderer() { return m_renderer; }
};

}

