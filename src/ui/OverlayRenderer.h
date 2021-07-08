#pragma once

#include "../Model.h"
#include "../ShaderProgram.h"
#include "../Texture.h"
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <memory>
#include <string>

class OverlayRenderer final
{
private:
    float m_windowRatio{1.0f};

    std::unique_ptr<ShaderProgram> m_fontShader;
    std::array<std::unique_ptr<Model>, 94> m_fontModels;

    std::unique_ptr<Model> m_crosshairModel;

    std::unique_ptr<ShaderProgram> m_uiShader;
    std::unique_ptr<Model> m_rectangleModel;

    std::unique_ptr<ShaderProgram> m_modelPreviewShader;

    class DrawCommand
    {
    protected: DrawCommand() {} // Make it non-constructible
    public:
        enum class Type
        {
            Rect,
            TextRender,
        } type;

        virtual ~DrawCommand() {} // Make `dynamic_cast` work
    };

    class RectDrawCommand : public DrawCommand
    {
    public:
        glm::vec2 position;
        glm::vec2 size;
        glm::vec3 color;
        
        RectDrawCommand() { type = Type::Rect; }
    };

    class TextRenderDrawCommand : public DrawCommand
    {
    public:
        std::string text;
        float fontScale;
        glm::vec2 textPos;
        glm::vec3 textColor;
        
        TextRenderDrawCommand() { type = Type::TextRender; }
    };

    std::vector<std::unique_ptr<DrawCommand>> m_drawCommands;

public:
    OverlayRenderer();

    void setWindowRatio(float val) { m_windowRatio = val; }

    bool open(const std::string& fontDirectoryPath, const std::string& crosshairModelPath);

    /*
     * Position: Offset of the rectangle's top left corner from the screen's top left corner.
     *           {0.0f, 0.0f} is in the left corner. {1.0f, 1.0f} is in the bottom right corner.
     * Size: The size of the rectangle. {1.0f, 1.0f} fills the whole screen.
     * Color: The color to fill the rectangle with.
     */
    void drawFilledRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color);

    void renderText(
            const std::string& text, float fontScale=0.1f, const glm::vec2& textPos={0.0f, 0.0f}, const glm::vec3& textColor={1.0f, 1.0f, 1.0f});

    void drawCrosshair();

    void commit();
};

