#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_opengl.h>
#include "Logger.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Model.h"

#define INITIAL_WIN_W 1500
#define INITIAL_WIN_H 1000
//#define WINDOW_FULLSCREEN_MODE SDL_WINDOW_FULLSCREEN_DESKTOP
#define WINDOW_FULLSCREEN_MODE 0

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        Logger::err << "Failed to initialize SDL: " << SDL_GetError() << Logger::End;
        return 1;
    }
    Logger::verb << "Initialized SDL" << Logger::End;

    auto window = SDL_CreateWindow(
            "OpenGL Engine",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            INITIAL_WIN_W, INITIAL_WIN_H,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | (WINDOW_FULLSCREEN_MODE ? WINDOW_FULLSCREEN_MODE : 0)
    );
    if (!window)
    {
        Logger::err << "Failed to create window: " << SDL_GetError() << Logger::End;
        return 1;
    }
    Logger::verb << "Created window" << Logger::End;

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)
     || SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)
     || SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE))
    {
        Logger::err << "Failed to set context attributes: " << SDL_GetError() << Logger::End;
        return 1;
    }
    auto context = SDL_GL_CreateContext(window);
    if (!context)
    {
        Logger::err << "Failed to create OpenGL context: " << SDL_GetError() << Logger::End;
        return 1;
    }
    if (SDL_GL_SetSwapInterval(1))
    {
        Logger::warn << "Failed to use V-Sync" << Logger::End;
    }
    Logger::verb << "Created and set up context" << Logger::End;

    glewExperimental = true;
    auto glewInitStatus = glewInit();
    if (glewInitStatus != GLEW_OK)
    {
        Logger::err << "Failed to initialize GLEW: " << glewGetErrorString(glewInitStatus) << Logger::End;
        return 1;
    }
    Logger::verb << "Initialized GLEW" << Logger::End;


    ShaderProgram shader;
    if (shader.open("../shaders/basic.vert.glsl", "../shaders/basic.frag.glsl"))
        return 1;
    shader.use();

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);
    Camera camera{{0.0f, 0.0f, 0.0f}, (float)winW/winH};
    camera.updateShaderUniforms(shader.getId());

    Model model;
    if (model.open("../models/monkey.obj"))
        return 1;

    auto modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, {0.0f, 0.0f, 0.0f});
    glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    bool shouldQuit = false;
    bool isInWireframeMode = false;
    while (!shouldQuit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event) && !shouldQuit)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                Logger::verb << "Window close event" << Logger::End;
                shouldQuit = true;
                break;

            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    glViewport(0, 0, event.window.data1, event.window.data2);
                    break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    shouldQuit = true;
                    break;

                case SDLK_F3:
                    isInWireframeMode = !isInWireframeMode;
                    glPolygonMode(GL_FRONT_AND_BACK, isInWireframeMode ? GL_LINE : GL_FILL);
                    break;
                }
                break;
            }
        }
        if (shouldQuit)
            break;

        auto keyboardState = SDL_GetKeyboardState(nullptr);
        if (keyboardState[SDL_SCANCODE_W])
            camera.moveForward(0.1f);
        else if (keyboardState[SDL_SCANCODE_S])
            camera.moveBackwards(0.1f);
        if (keyboardState[SDL_SCANCODE_A])
            camera.moveLeft(0.1f);
        else if (keyboardState[SDL_SCANCODE_D])
            camera.moveRight(0.1f);

        // TODO: Remove these lines when things work
        {
            auto camPos = camera.getPosition();
            std::string title = std::to_string(camPos.x) + ", " + std::to_string(camPos.y) + ", " + std::to_string(camPos.z);
            SDL_SetWindowTitle(window, title.c_str());
        }

        camera.updateShaderUniforms(shader.getId());

        // TODO: Remove these lines when things work
        {
            camera.setYawDeg(camera.getYawDeg() + 1);
            camera.setPitchDeg(camera.getPitchDeg() + 1);
            camera.recalculateFrontVector();
        }


        glClearColor(0.34f, 0.406f, 0.642f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        model.draw();


        SDL_GL_SwapWindow(window);
    }


    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    Logger::verb << "Cleaned up" << Logger::End;
    return 0;
}

