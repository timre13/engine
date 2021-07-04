#include "TextRenderer.h"
#include "Logger.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <memory>
#include <glm/gtc/type_ptr.hpp>

#define GUI_BG_R 0.082f
#define GUI_BG_G 0.142f
#define GUI_BG_B 0.249f

OverlayRenderer::OverlayRenderer()
{
    m_fontShader = std::make_unique<ShaderProgram>();
    for (size_t i{}; i < m_fontModels.size(); ++i)
        m_fontModels[i] = std::make_unique<Model>();

    m_crosshairModel = std::make_unique<Model>();

    m_uiShader = std::make_unique<ShaderProgram>();
    m_rectangleModel = std::make_unique<Model>();
}

bool OverlayRenderer::open(const std::string& fontDirectoryPath, const std::string& crosshairModelPath)
{
    Logger::verb << "Opening font shaders" << Logger::End;
    if (m_fontShader->open("../shaders/font.vert.glsl", "../shaders/font.frag.glsl"))
    {
        return 1;
    }

    Logger::verb << "Opening font: " << fontDirectoryPath << Logger::End;
    for (char i{'!'}; i < '~'; ++i)
    {
        if (m_fontModels[i-'!']->open(fontDirectoryPath+"/"+std::to_string(i)+".obj"))
            return 1;
    }
    Logger::verb << "Opened font" << Logger::End;

    if (m_crosshairModel->open(crosshairModelPath))
    {
        return 1;
    }
    Logger::verb << "Opened crosshair model" << Logger::End;


    Logger::verb << "Opening UI shaders" << Logger::End;
    if (m_uiShader->open("../shaders/ui.vert.glsl", "../shaders/ui.frag.glsl"))
    {
        return 1;
    }
    m_rectangleModel->open("../models/plane.obj");

    return 0;
}

void OverlayRenderer::drawCrosshair(float windowRatio)
{
    m_fontShader->use();

    static constexpr float scale = 0.04f;
    glUniform1f(glGetUniformLocation(m_fontShader->getId(), "horizontalFontScale"), scale);
    glUniform1f(glGetUniformLocation(m_fontShader->getId(), "verticalFontScale"), scale*windowRatio);
    glUniform3f(glGetUniformLocation(m_fontShader->getId(), "characterColor"), 1.0f, 1.0f, 1.0f);
    glUniform2f(glGetUniformLocation(m_fontShader->getId(), "characterPos"), 1.0f/scale, -(1.0f/scale)/windowRatio);

    m_crosshairModel->draw();
}

void OverlayRenderer::renderText(
        const std::string& text, float fontScale/*=0.1f*/, const glm::vec2& textPos/*={0.0f, 0.0f}*/, const glm::vec3& textColor/*={1.0f, 1.0f, 1.0f}*/)
{
    m_fontShader->use();
    glUniform1f(glGetUniformLocation(m_fontShader->getId(), "horizontalFontScale"), fontScale);
    glUniform1f(glGetUniformLocation(m_fontShader->getId(), "verticalFontScale"), fontScale);

    glUniform3f(glGetUniformLocation(m_fontShader->getId(), "characterColor"), textColor.r, textColor.g, textColor.b);

    static constexpr float characterWidth = 0.25f;
    static constexpr float characterHeight = 0.4f;

    glm::vec2 characterPos{textPos};
    for (size_t i{}; i < text.size(); ++i)
    {
        unsigned char c = text[i];

        if (c >= '!' && c <= '~')
        {
            glUniform2f(glGetUniformLocation(m_fontShader->getId(), "characterPos"), characterPos.x, characterPos.y);
            m_fontModels[c-'!']->draw();
            characterPos.x += characterWidth;
        }
        else
        {
            switch (c)
            {
            case '\n':
                characterPos.x = textPos.x;
                characterPos.y -= characterHeight;
                break;

            case '\r':
                characterPos.x = textPos.x;
                break;

            case '\t':
                characterPos.x += characterWidth*4;
                break;

            case '\v':
                characterPos.y -= characterWidth*4;
                break;

            default: // Space and others
                characterPos.x += characterWidth;
                break;
            }
        }

        if (characterPos.x*fontScale >= 2.0f)
        {
            characterPos.x = textPos.x;
            characterPos.y -= characterHeight;
        }
    }
}

void OverlayRenderer::drawFilledRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color)
{
    m_uiShader->use();
    glUniform2f(glGetUniformLocation(m_uiShader->getId(), "position"), position.x, position.y);
    glUniform2f(glGetUniformLocation(m_uiShader->getId(), "size"), size.x, size.y);
    glUniform3f(glGetUniformLocation(m_uiShader->getId(), "uiColor"), color.r, color.g, color.b);
    m_rectangleModel->draw();
}

void OverlayRenderer::drawBuildMenu()
{
    drawFilledRectangle({0.0f, 0.0f}, {0.5f, 0.5f}, {GUI_BG_R, GUI_BG_G, GUI_BG_B});
}

