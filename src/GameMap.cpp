#include "GameMap.h"
#include "Logger.h"

GameMap::GameMap()
{
#if 1 // ----- TEST -----
    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Teapot 1",
            .modelName = "teapot.obj",
            .textureName = "concrete/4K-concrete_50_basecolor.png",
            .xPos = 0.0f,
            .yPos = 0.0f,
            .zPos = 0.0f,
            .scaleX = 0.05f,
            .scaleY = 0.05f,
            .scaleZ = 0.05f,
    }});

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Teapot 2",
            .modelName = "teapot.obj",
            .textureName = "concrete/4K-concrete_50_basecolor.png",
            .xPos = 7.0f,
            .yPos = 3.0f,
            .zPos = 4.0f,
            .scaleX = 0.02f,
            .scaleY = 0.02f,
            .scaleZ = 0.02f,
    }});

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Cube",
            .modelName = "cube.obj",
            .textureName = "placeholder.png",
            .xPos = 2.5f,
            .yPos = 4.0f,
            .zPos = 2.3f,
            .scaleX = 0.5f,
            .scaleY = 0.5f,
            .scaleZ = 0.5f,
    }});

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Quake Guy",
            .modelName = "private/player.obj",
            .textureName = "private/player_0.png",
            .xPos = -3.0f,
            .yPos = 0.0f,
            .zPos = 0.0f,
            .scaleX = 0.005f,
            .scaleY = 0.005f,
            .scaleZ = 0.005f,
    }});
#endif

    Logger::log << "Loaded map with " << m_objects.size() << " objects" << Logger::End;
}
