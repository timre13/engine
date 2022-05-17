#pragma once

#include <vector>
#include <memory>
#include <string>
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
        float                   xPos{};
        float                   yPos{};
        float                   zPos{};
        float                   scaleX = 1.0f;
        float                   scaleY = 1.0f;
        float                   scaleZ = 1.0f;
    };

    using objectList_t = std::vector<std::unique_ptr<ObjectDescr>>;
    objectList_t m_objects;

public:
    GameMap();

    inline const objectList_t& getObjects() const { return m_objects; }
};
