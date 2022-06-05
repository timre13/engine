#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include <bullet/BulletCollision/btBulletCollisionCommon.h>
#include "GameObject.h"

class GameMap final
{
public:
    struct MapFormatVer
    {
        uint major;
        uint minor;
    };

    struct ObjectDescr
    {
        std::string             objName = "<Object>";
        std::string             modelName; // Required
        std::string             textureName; // Required
        GameObject::flag_t      flags = GameObject::defaultFlags;

        glm::vec3               pos{}; // Required
        glm::vec3               scale{1.0f, 1.0f, 1.0f};
        glm::vec3               modelRotDeg{0.0f, 0.0f, 0.0f};

        std::unique_ptr<btCollisionShape> collShape;
        btScalar                mass = 0.0f;
    };
    using objectList_t = std::vector<std::unique_ptr<ObjectDescr>>;

private:
    std::string m_name; // Required
    std::string m_descr = "N/A";
    std::string m_author = "<unknown>";

    MapFormatVer m_mapFormatVer; // Required

    objectList_t m_objects; // Required

public:
    GameMap(const std::string& path);

    inline const objectList_t& getObjects() const { return m_objects; }
};
