#pragma once

#include "../ShaderProgram.h"
#include "../Texture.h"
#include <glm/glm.hpp>
#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

#define DEF_FONT_SIZE 16

class Model;

namespace UI
{

class OverlayRenderer final
{
private:
    int m_windowWidth{};
    int m_windowHeight{};
    float m_windowRatio{1.0f};

    std::unique_ptr<ShaderProgram> m_fontShader;
    struct Character
    {
        uint textureId;
        glm::ivec2 size;
        glm::ivec2 bearing;
        uint advance;
    };
    std::map<char, Character> m_characters;
    uint m_fontVBO{};
    uint m_fontVAO{};

    //std::unique_ptr<Model> m_crosshairModel;

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
            Text,
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

    class TextDrawCommand : public DrawCommand
    {
    public:
        std::string text;
        float scale;
        glm::vec2 textPos;
        glm::vec3 textColor;
        
        TextDrawCommand() { type = Type::Text; }
    };

    std::vector<std::unique_ptr<DrawCommand>> m_drawCommands;

public:
    OverlayRenderer();

    void setWindowSize(int width, int height) { m_windowRatio = (float)width/height; m_windowWidth = width; m_windowHeight = height; }

    bool construct(const std::string& crosshairModelPath);

    /*
     * Position: Offset of the rectangle's top left corner from the screen's top left corner.
     *           {0.0f, 0.0f} is in the top left corner. {1.0f, 1.0f} is in the bottom right corner.
     * Size: The size of the rectangle. {1.0f, 1.0f} fills the whole screen.
     * Color: The color to fill the rectangle with.
     */
    void drawFilledRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color);

    void renderText(
            const std::string& text, float scale, const glm::vec2& textPos, const glm::vec3& textColor={1.0f, 1.0f, 1.0f});

    void drawCrosshair();

    void commit();
};

} // namespace UI

