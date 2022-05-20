#include "GameMap.h"
#include "Logger.h"

GameMap::GameMap()
{
#if 1 // ----- TEST -----
    static constexpr float floorScale = 4.0f;
    for (int x{-100}; x <= 100; ++x)
    {
        for (int y{-100}; y <= 100; ++y)
        {
            m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
                    .objName = "Plane",
                    .modelName = "plane.obj",
                    .textureName = "floor_tiled_stone/square_floor_diff_4k.jpg",
                    .pos = {
                        x*floorScale,
                        0.0f,
                        y*floorScale,
                    },
                    .scale = {
                        floorScale,
                        floorScale,
                        floorScale,
                    },
            }});
        }
    }

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Teapot 1",
            .modelName = "teapot.obj",
            .textureName = "concrete/4K-concrete_50_basecolor.png",
            .pos = {
                2.5f,
                0.0f,
                2.3f,
            },
            .scale = {
                0.05f,
                0.05f,
                0.05f,
            },
    }});

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Chair",
            .modelName = "chair.obj",
            .textureName = "afromosia/2K_afromosia_basecolor.png",
            .pos = {
                2.0f,
                0.0f,
                4.0f,
            },
            .scale = {
                1.0f,
                1.0f,
                1.0f,
            },
    }});

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Cube",
            .modelName = "cube.obj",
            .textureName = "",
            .pos = {
                0.0f,
                0.1f,
                0.0f,
            },
            .scale = {
                1.0f,
                1.0f,
                1.0f,
            },
    }});

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Cube",
            .modelName = "cube.obj",
            .textureName = "",
            .pos = {
                1.0f,
                0.1f,
                1.0f,
            },
            .scale = {
                1.0f,
                1.0f,
                1.0f,
            },
    }});

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Bucket",
            .modelName = "bucket.obj",
            .textureName = "bucket/Bucket3Albedo.png",
            .pos = {
                1.5f,
                0.0f,
                -3.0f,
            },
            .scale = {
                1.0f,
                1.0f,
                1.0f,
            },
    }});

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Quake Guy",
            .modelName = "private/player.obj",
            .textureName = "private/player_0.png",
            .pos = {
                -3.0f,
                1.2f,
                0.0f,
            },
            .scale = {
                0.05f,
                0.05f,
                0.05f,
            },
    }});
#endif

    Logger::log << "Loaded map with " << m_objects.size() << " objects" << Logger::End;
}
