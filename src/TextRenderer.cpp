#include "TextRenderer.h"
#include "Logger.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <memory>
#include <glm/gtc/type_ptr.hpp>

TextRenderer::TextRenderer()
{
    m_shader = std::make_unique<ShaderProgram>();

    for (size_t i{}; i < m_models.size(); ++i)
        m_models[i] = std::make_unique<Model>();
}

bool TextRenderer::openFont(const std::string& directoryPath)
{
    Logger::verb << "Opening font shaders" << Logger::End;

    if (m_shader->open("../shaders/font.vert.glsl", "../shaders/font.frag.glsl"))
    {
        return 1;
    }

    Logger::verb << "Opening font: " << directoryPath << Logger::End;

    for (char i{'!'}; i < '~'; ++i)
    {
        if (m_models[i-'!']->open(directoryPath+"/"+std::to_string(i)+".obj"))
            return 1;
    }

    Logger::verb << "Opened font" << Logger::End;
    return 0;
}


void TextRenderer::renderText(
        const std::string& text, float fontScale/*=0.1f*/, const glm::vec2& textPos/*={0.0f, 0.0f}*/, const glm::vec3& textColor/*={1.0f, 1.0f, 1.0f}*/)
{
    m_shader->use();
    glUniform1f(glGetUniformLocation(m_shader->getId(), "fontScale"), fontScale);

    glUniform3f(glGetUniformLocation(m_shader->getId(), "characterColor"), textColor.r, textColor.g, textColor.b);

    static constexpr float characterWidth = 0.25f;
    static constexpr float characterHeight = 0.4f;

    glm::vec2 characterPos{textPos};
    for (size_t i{}; i < text.size(); ++i)
    {
        unsigned char c = text[i];

        if (c >= '!' && c <= '~')
        {
            glUniform2f(glGetUniformLocation(m_shader->getId(), "characterPos"), characterPos.x, characterPos.y);
            m_models[c-'!']->draw();
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

