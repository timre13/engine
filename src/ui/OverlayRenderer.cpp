#include "OverlayRenderer.h"
#include "../Logger.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include "colors.h"

OverlayRenderer::OverlayRenderer()
{
    m_fontShader = std::make_unique<ShaderProgram>();
    for (size_t i{}; i < m_fontModels.size(); ++i)
        m_fontModels[i] = std::make_unique<Model>();

    m_crosshairModel = std::make_unique<Model>();

    m_uiShader = std::make_unique<ShaderProgram>();
    m_rectangleModel = std::make_unique<Model>();
    
    m_modelPreviewShader = std::make_unique<ShaderProgram>();
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


    Logger::verb << "Opening model preview shaders" << Logger::End;
    if (m_modelPreviewShader->open("../shaders/build_menu.vert.glsl", "../shaders/build_menu.frag.glsl"))
    {
        return 1;
    }

    return 0;
}

void OverlayRenderer::drawCrosshair()
{
    m_fontShader->use();

    static constexpr float scale = 0.04f;
    glUniform1f(glGetUniformLocation(m_fontShader->getId(), "horizontalFontScale"), scale);
    glUniform1f(glGetUniformLocation(m_fontShader->getId(), "verticalFontScale"), scale*m_windowRatio);
    glUniform3f(glGetUniformLocation(m_fontShader->getId(), "characterColor"), 1.0f, 1.0f, 1.0f);
    glUniform2f(glGetUniformLocation(m_fontShader->getId(), "characterPos"), 1.0f/scale, -(1.0f/scale)/m_windowRatio);

    m_crosshairModel->draw();
}

void OverlayRenderer::renderText(
        const std::string& text, float fontScale/*=0.1f*/, const glm::vec2& textPos/*={0.0f, 0.0f}*/, const glm::vec3& textColor/*={1.0f, 1.0f, 1.0f}*/)
{
    auto command = std::make_unique<TextRenderDrawCommand>();
    command->text = text;
    command->fontScale = fontScale;
    command->textPos = textPos;
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
        auto& c = *it;

        switch (c->type)
        {
        case DrawCommand::Type::Rect:
        {
            auto cmd = dynamic_cast<RectDrawCommand*>(c.get());

            m_uiShader->use();
            glUniform2f(glGetUniformLocation(m_uiShader->getId(), "position"), cmd->position.x*2-1.0f+cmd->size.x, cmd->position.y*2-1.0f+cmd->size.y);
            glUniform2f(glGetUniformLocation(m_uiShader->getId(), "size"), cmd->size.x, cmd->size.y);
            glUniform3f(glGetUniformLocation(m_uiShader->getId(), "uiColor"), cmd->color.r, cmd->color.g, cmd->color.b);
            m_rectangleModel->draw();

            break;
        }

        case DrawCommand::Type::TextRender:
        {
            auto cmd = dynamic_cast<TextRenderDrawCommand*>(c.get());

            m_fontShader->use();
            glUniform1f(glGetUniformLocation(m_fontShader->getId(), "horizontalFontScale"), cmd->fontScale);
            glUniform1f(glGetUniformLocation(m_fontShader->getId(), "verticalFontScale"), cmd->fontScale);
        
            glUniform3f(glGetUniformLocation(m_fontShader->getId(), "characterColor"), cmd->textColor.r, cmd->textColor.g, cmd->textColor.b);
        
            static constexpr float characterWidth = 0.25f;
            static constexpr float characterHeight = 0.4f;
        
            glm::vec2 characterPos{cmd->textPos};
            for (size_t i{}; i < cmd->text.size(); ++i)
            {
                unsigned char c = cmd->text[i];
        
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
                        characterPos.x = cmd->textPos.x;
                        characterPos.y -= characterHeight;
                        break;
        
                    case '\r':
                        characterPos.x = cmd->textPos.x;
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
        
                if (characterPos.x*cmd->fontScale >= 2.0f)
                {
                    characterPos.x = cmd->textPos.x;
                    characterPos.y -= characterHeight;
                }
            }
            break;
        }
        }
    }

    m_drawCommands.clear();
}

