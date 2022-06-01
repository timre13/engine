#include "GameMap.h"
#include "Logger.h"

GameMap::GameMap()
{
#if 1 // ----- TEST -----
    static constexpr float floorScale = 20.0f;
    //int x = 0;
    //int y = 0;
    //for (int x{-100}; x <= 100; ++x)
    //{
    //    for (int y{-100}; y <= 100; ++y)
    //    {
            m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
                    .objName = "Plane",
                    .modelName = "plane.obj",
                    .textureName = "floor_tiled_stone/square_floor_diff_4k.jpg",
                    .pos = {
                        //x*floorScale,
                        0.0f,
                        0.0f,
                        //y*floorScale,
                        0.0f,
                    },
                    .scale = {
                        floorScale,
                        1.0f,
                        floorScale,
                    },
                    .collShape{new btBoxShape{btVector3{floorScale/2, .0005f, floorScale/2}}},
                    .mass = 0.f,
            }});
    //    }
    //}

    //m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
    //        .objName = "Teapot 1",
    //        .modelName = "teapot.obj",
    //        .textureName = "concrete/4K-concrete_50_basecolor.png",
    //        .pos = {
    //            2.5f,
    //            100.0f,
    //            2.3f,
    //        },
    //        .scale = {
    //            //0.05f,
    //            //0.05f,
    //            //0.05f,
    //            1.0f,
    //            1.0f,
    //            1.0f,
    //        },
    //        .collShape = new btBoxShape{btVector3{1.f, 1.f, 1.f}},
    //        //.collShape = new btSphereShape{0.5f},
    //        .mass = 1.f,
    //}});

    //m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
    //        .objName = "Chair",
    //        .modelName = "chair.obj",
    //        .textureName = "afromosia/2K_afromosia_basecolor.png",
    //        .pos = {
    //            2.0f,
    //            0.0f,
    //            4.0f,
    //        },
    //        .scale = {
    //            1.0f,
    //            1.0f,
    //            1.0f,
    //        },
    //}});

    for (int x{}; x < 6; ++x)
    {
        for (int z{}; z < 6; ++z)
        {
            for (int y{}; y < 10; ++y)
            {
                m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
                        .objName = "Cube",
                        .modelName = "cube.obj",
                        .textureName = "private/gaben.jpeg",
                        .pos = {
                            0.0f+x*1.01f,
                            0.0f+y*1.01f,
                            0.0f+z*1.01f,
                        },
                        .scale = {
                            1.0f,
                            1.0f,
                            1.0f,
                        },
                        .modelRotDeg = {
                            90.0f,
                            90.0f,
                            0.0f,
                        },
                        .collShape{new btBoxShape{btVector3{.5f, .5f, .5f}}},
                        .mass = 1.f,
                }});
            }
#if 0
        }
    }
#endif

    m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
            .objName = "Sphere",
            .modelName = "ico_sphere.obj",
            .textureName = "private/hl3.jpg",
            .pos = {
                3.0f,
                100.0f,
                3.0f,
            },
            .scale = {
                4.0f,
                4.0f,
                4.0f,
            },
            .collShape{new btSphereShape{1.85f}},
            .mass = 10.0f,
    }});


    //m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
    //        .objName = "Cube",
    //        .modelName = "cube.obj",
    //        .textureName = "",
    //        .pos = {
    //            1.0f,
    //            0.1f,
    //            1.0f,
    //        },
    //        .scale = {
    //            1.0f,
    //            1.0f,
    //            1.0f,
    //        },
    //}});

    //m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
    //        .objName = "Bucket",
    //        .modelName = "bucket.obj",
    //        .textureName = "bucket/Bucket3Albedo.png",
    //        .pos = {
    //            1.5f,
    //            0.0f,
    //            -3.0f,
    //        },
    //        .scale = {
    //            1.0f,
    //            1.0f,
    //            1.0f,
    //        },
    //}});

    //m_objects.push_back(std::unique_ptr<ObjectDescr>{new ObjectDescr{
    //        .objName = "Quake Guy",
    //        .modelName = "private/player.obj",
    //        .textureName = "private/player_0.png",
    //        .pos = {
    //            -3.0f,
    //            1.2f,
    //            0.0f,
    //        },
    //        .scale = {
    //            0.05f,
    //            0.05f,
    //            0.05f,
    //        },
    //}});
#endif

    Logger::log << "Loaded map with " << m_objects.size() << " objects" << Logger::End;
}
