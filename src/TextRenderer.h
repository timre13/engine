#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include <glm/glm.hpp>
#include <array>
#include <memory>
#include <string>

class OverlayRenderer final
{
private:
    std::unique_ptr<ShaderProgram> m_fontShader;
    std::array<std::unique_ptr<Model>, 94> m_fontModels;

    std::unique_ptr<Model> m_crosshairModel;

    std::unique_ptr<ShaderProgram> m_uiShader;
    std::unique_ptr<Model> m_rectangleModel;

    void drawFilledRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color);

public:
    OverlayRenderer();

    bool open(const std::string& fontDirectoryPath, const std::string& crosshairModelPath);
    void renderText(
            const std::string& text, float fontScale=0.1f, const glm::vec2& textPos={0.0f, 0.0f}, const glm::vec3& textColor={1.0f, 1.0f, 1.0f});
    void drawCrosshair(float windowRatio);
    void drawBuildMenu();
};

