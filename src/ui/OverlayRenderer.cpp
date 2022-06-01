#include "OverlayRenderer.h"
#include "../Model.h"
#include "../Logger.h"
#include "colors.h"
#include "../os.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <sstream>
#include <fstream>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace UI
{

OverlayRenderer::OverlayRenderer()
{
    m_fontShader = std::make_unique<ShaderProgram>();

    //m_crosshairModel = std::make_unique<Model>();

    m_uiShader = std::make_unique<ShaderProgram>();
    
    m_modelPreviewShader = std::make_unique<ShaderProgram>();
}

static void setUpFont(std::map<char, OverlayRenderer::Character>* characters, uint* fontVAO, uint* fontVBO)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        Logger::err << "Failed to initialize FreeType library" << Logger::End;
        abort();
    }

    std::string fontFilePath = OS::getFontFilePath("DejaVu Sans Mono");
    if (fontFilePath.empty())
    {
        Logger::err << "Failed to get font file path" << Logger::End;
        abort();
    }
    Logger::verb << "Loading font: " << fontFilePath << Logger::End;
    FT_Face face;
    if (FT_New_Face(ft, fontFilePath.c_str(), 0, &face))
    {
        Logger::err << "Failed to load font" << Logger::End;
        abort();
    }
    if (FT_Set_Pixel_Sizes(face, 0, DEF_FONT_SIZE))
    {
        Logger::err << "Failed to set font size" << Logger::End;
        abort();
    }

    Logger::verb << "Caching glyphs" << Logger::End;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (int c{}; c < 128; ++c)
    {
        // Activate the current character
        if (FT_Load_Char(face, (char)c, FT_LOAD_RENDER))
        {
            Logger::err << "Failed to load glyph for character: " << c << Logger::End;
            abort();
        }

        // Move the texture to the VRAM
        uint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RED,
                face->glyph->bitmap.width, face->glyph->bitmap.rows,
                0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store the character
        characters->insert({(char)c, {
                    textureId, // Texture
                    {face->glyph->bitmap.width, face->glyph->bitmap.rows}, // Size
                    {face->glyph->bitmap_left, face->glyph->bitmap_top}, // Bearing
                    (uint)face->glyph->advance.x // Advance
        }});
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, fontVAO);
    glBindVertexArray(*fontVAO);
    glGenBuffers(1, fontVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *fontVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool OverlayRenderer::construct(const std::string& crosshairModelPath)
{
    Logger::verb << "Opening font shaders" << Logger::End;
    if (m_fontShader->open("../shaders/font.vert.glsl", "../shaders/font.frag.glsl"))
    {
        return 1;
    }
    setUpFont(&m_characters, &m_fontVAO, &m_fontVBO);


    //if (m_crosshairModel->open(crosshairModelPath))
    //{
    //    return 1;
    //}
    //Logger::verb << "Opened crosshair model" << Logger::End;


    Logger::verb << "Opening UI shaders" << Logger::End;
    if (m_uiShader->open("../shaders/ui.vert.glsl", "../shaders/ui.frag.glsl"))
    {
        return 1;
    }
    m_uiShader->use();
    const auto matrix = glm::ortho(0.0f, 100.0f, 0.0f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(m_uiShader->getId(), "projectionMat"), 1, GL_FALSE, glm::value_ptr(matrix));

    // Create VAO and VBO for the UI rectangle
    glGenVertexArrays(1, &m_uiVAO);
    glBindVertexArray(m_uiVAO);
    glGenBuffers(1, &m_uiVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    Logger::verb << "Opening model preview shaders" << Logger::End;
    if (m_modelPreviewShader->open("../shaders/build_menu.vert.glsl", "../shaders/build_menu.frag.glsl"))
    {
        return 1;
    }

    return 0;
}

void OverlayRenderer::drawCrosshair()
{
    //m_fontShader->use();

    //static constexpr float scale = 0.04f;
    //glUniform1f(glGetUniformLocation(m_fontShader->getId(), "horizontalFontScale"), scale);
    //glUniform1f(glGetUniformLocation(m_fontShader->getId(), "verticalFontScale"), scale*m_windowRatio);
    //glUniform3f(glGetUniformLocation(m_fontShader->getId(), "characterColor"), 1.0f, 1.0f, 1.0f);
    //glUniform2f(glGetUniformLocation(m_fontShader->getId(), "characterPos"), 1.0f/scale, -(1.0f/scale)/m_windowRatio);

    //m_crosshairModel->draw();
}

void OverlayRenderer::renderTextAtPx(
        const std::string& text, float scale, const glm::ivec2& textPos, const glm::vec3& textColor/*={1.0f, 1.0f, 1.0f}*/)
{
    auto command = std::make_unique<TextDrawCommand>();
    command->text = text;
    command->scale = scale;
    command->textPos = textPos;
    command->textColor = textColor;
    m_drawCommands.push_back(std::move(command));
}

void OverlayRenderer::renderTextAtPerc(
        const std::string& text, float scale, const glm::vec2& textPos, const glm::vec3& textColor/*={1.0f, 1.0f, 1.0f}*/)
{
    auto command = std::make_unique<TextDrawCommand>();
    command->text = text;
    command->scale = scale;
    command->textPos = {m_windowWidth*textPos.x/100, m_windowHeight*textPos.y/100*m_windowRatio};
    command->textColor = textColor;
    m_drawCommands.push_back(std::move(command));
}

void OverlayRenderer::drawFilledRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color)
{
    auto command = std::make_unique<RectDrawCommand>();
    command->position = {position.x, position.y*m_windowRatio};
    command->size = {size.x, size.y*m_windowRatio};
    command->color = color;
    m_drawCommands.push_back(std::move(command));
}

void OverlayRenderer::commit()
{
    for (auto it = m_drawCommands.rbegin(); it != m_drawCommands.rend(); ++it)
    {
        const auto& c = *it;

        switch (c->type)
        {
        case DrawCommand::Type::Rect:
        {
            const auto cmd = dynamic_cast<RectDrawCommand*>(c.get());
            m_uiShader->use();

            glUniform3f(glGetUniformLocation(m_uiShader->getId(), "uiColor"), cmd->color.r, cmd->color.g, cmd->color.b);

            const float vertices[6][2] = {
                {cmd->position.x,               cmd->position.y},
                {cmd->position.x,               cmd->position.y + cmd->size.y},
                {cmd->position.x + cmd->size.x, cmd->position.y + cmd->size.y},
                {cmd->position.x + cmd->size.x, cmd->position.y + cmd->size.y},
                {cmd->position.x + cmd->size.x, cmd->position.y},
                {cmd->position.x,               cmd->position.y},
            };

            glBindVertexArray(m_uiVAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);
            break;
        }

        case DrawCommand::Type::Text:
        {
            const auto cmd = dynamic_cast<TextDrawCommand*>(c.get());
            float textX = cmd->textPos.x;
            float textY = cmd->textPos.y;

            m_fontShader->use();
            glUniform3f(glGetUniformLocation(m_fontShader->getId(), "textColor"), cmd->textColor.r, cmd->textColor.g, cmd->textColor.b);
            // The matrix size will be = to the window size, so we can use pixels as size
            const auto matrix = glm::ortho(0.0f, (float)m_windowWidth, 0.0f, (float)m_windowHeight);
            glUniformMatrix4fv(glGetUniformLocation(m_fontShader->getId(), "projectionMat"), 1, false, glm::value_ptr(matrix));
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(m_fontVAO);

            for (auto it = cmd->text.begin(); it != cmd->text.end(); ++it)
            {
                switch (*it)
                {
                case '\n':
                    textX = cmd->textPos.x;
                    textY -= (float)DEF_FONT_SIZE;
                    break;

                case '\t':
                    textX += (float)DEF_FONT_SIZE*4;
                    break;
 
                default: // Printable char
                    const Character& ch = m_characters[*it];

                    const float charXPos = textX + ch.bearing.x * cmd->scale;
                    const float charYPos = textY - (ch.size.y - ch.bearing.y) * cmd->scale;
                    const float charWidth = ch.size.x * cmd->scale;
                    const float charHeight = ch.size.y * cmd->scale;

                    const float vertices[6][4] = {
                        {charXPos,             charYPos + charHeight, 0.0f, 0.0f},
                        {charXPos,             charYPos,              0.0f, 1.0f},
                        {charXPos + charWidth, charYPos,              1.0f, 1.0f},

                        {charXPos,             charYPos + charHeight, 0.0f, 0.0f},
                        {charXPos + charWidth, charYPos,              1.0f, 1.0f},
                        {charXPos + charWidth, charYPos + charHeight, 1.0f, 0.0f},
                    };

                    glBindTexture(GL_TEXTURE_2D, ch.textureId);

                    glBindBuffer(GL_ARRAY_BUFFER, m_fontVBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);

                    glDrawArrays(GL_TRIANGLES, 0, 6);

                    textX += (ch.advance/64.f) * cmd->scale;

                    break;
                }
            }

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            break;
        }
        }
    }

    m_drawCommands.clear();
}

} // namespace UI

