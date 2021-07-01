#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include <glm/glm.hpp>
#include <array>
#include <memory>
#include <string>

class TextRenderer final
{
private:
    std::unique_ptr<ShaderProgram> m_shader;
    std::array<std::unique_ptr<Model>, 94> m_models;

public:
    TextRenderer();

    bool openFont(const std::string& directoryPath);
    void renderText(
            const std::string& text, float fontScale=0.1f, const glm::vec2& textPos={0.0f, 0.0f}, const glm::vec3& textColor={1.0f, 1.0f, 1.0f});
};

