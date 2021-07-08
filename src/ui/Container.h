#include "Widget.h"

namespace UI
{

class Container : public Widget
{
protected:
    using childList_t = std::vector<std::shared_ptr<Widget>>;
    childList_t m_children{};

public:
    virtual void addChild(std::shared_ptr<Widget> child) { m_children.push_back(child); child->setParent(this); }
    virtual const childList_t& children() const { return m_children; }

    virtual void onCursorMove(float mouseX, float mouseY) override;
    virtual void onMouseClick(float clickX, float clickY) override;
};

}

