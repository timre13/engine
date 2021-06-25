#include "GameObject.h"

GameObject::GameObject(std::shared_ptr<Model> model, const std::string& objectName/*="Object"*/, flag_t flags/*=defaultFlags*/)
    : m_model{model}, m_name{objectName}, m_flags{flags}
{
}

