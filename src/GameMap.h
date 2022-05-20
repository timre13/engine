#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include "GameObject.h"

class GameMap final
{
private:
    struct ObjectDescr
    {
        std::string             objName;
        std::string             modelName;
        std::string             textureName;
        GameObject::flag_t      flags = GameObject::defaultFlags;

        glm::vec3               pos{1.0f, 1.0f, 1.0f};
        glm::vec3               scale{1.0f, 1.0f, 1.0f};
    };

    using objectList_t = std::vector<std::unique_ptr<ObjectDescr>>;
    objectList_t m_objects;

public:
    GameMap();

    inline const objectList_t& getObjects() const { return m_objects; }
};
