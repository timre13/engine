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
public:
    struct Character
    {
        uint textureId;
        glm::ivec2 size;
        glm::ivec2 bearing;
        uint advance;
    };

private:
    int m_windowWidth{};
    int m_windowHeight{};
    float m_windowRatio{1.0f};

    std::unique_ptr<ShaderProgram> m_fontShader;
    std::map<char, Character> m_characters;
    uint m_fontVBO{};
    uint m_fontVAO{};

    //std::unique_ptr<Model> m_crosshairModel;

    std::unique_ptr<ShaderProgram> m_uiShader;
    uint m_uiVBO{};
    uint m_uiVAO{};

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
        glm::ivec2 textPos;
        glm::vec3 textColor;
        
        TextDrawCommand() { type = Type::Text; }
    };

    std::vector<std::unique_ptr<DrawCommand>> m_drawCommands;

public:
    OverlayRenderer();

    void setWindowSize(int width, int height) { m_windowRatio = (float)width/height; m_windowWidth = width; m_windowHeight = height; }

    inline int getWindowWidth() const { return m_windowWidth; }
    inline int getWindowHeight() const { return m_windowHeight; }
    inline float getWindowRatio() const { return m_windowRatio; }

    bool construct(const std::string& crosshairModelPath);

    /*
     * position: Offset of the rectangle's bottom left corner from the screen's bottom left corner as percentage.
     *           {0.0f, 0.0f} is in the bottom left corner. {100.0f, 100.0f} is in the top right corner.
     * size: The size of the rectangle as percentage. {100.0f, 100.0f} fills the whole screen.
     * color: The color to fill the rectangle with.
     */
    void drawFilledRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color);

    /*
     * text: The text string to render.
     * scale: Size of the font relative to `DEF_FONT_SIZE`.
     * textPos: The position of the text on the screen in _PIXELS_. Relative to the bottom left corner.
     * textColor: The color to use to paint the text.
     */
    void renderTextAtPx(
            const std::string& text, float scale, const glm::ivec2& textPos, const glm::vec3& textColor={1.0f, 1.0f, 1.0f});

    /*
     * text: The text string to render.
     * scale: Size of the font relative to `DEF_FONT_SIZE`.
     * textPos: The position of the text on the screen in _PERCENTAGE_. Relative to the bottom left corner.
     * textColor: The color to use to paint the text.
     */
    void renderTextAtPerc(
            const std::string& text, float scale, const glm::vec2& textPos, const glm::vec3& textColor={1.0f, 1.0f, 1.0f});

    void drawCrosshair();

    void commit();
};

} // namespace UI

