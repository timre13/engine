#pragma once

#include <cstddef>

namespace Assets
{

constexpr size_t modelCount = 3;
constexpr const char* const models[modelCount] = {
    "../models/monkey_smooth2.obj",
    "../models/cube.obj",
    "../models/well.obj"
};

constexpr size_t textureCount = 3;
constexpr const char* const textures[textureCount] = {
    "../textures/monkey_texture.png",
    "../textures/concrete/4K-concrete_50_basecolor.png",
    "../textures/well.png"
};

}

