#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "Logger.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "GameObject.h"
#include "assets.h"
#include "Model.h"
#include "Texture.h"
#include "TextRenderer.h"

#define INITIAL_WIN_W 1500
#define INITIAL_WIN_H 1000
//#define WINDOW_FULLSCREEN_MODE SDL_WINDOW_FULLSCREEN_DESKTOP
#define WINDOW_FULLSCREEN_MODE 0
#define MOUSE_SENS 0.1f
#define USE_VSYNC 1
#define FOV_DEFAULT 45.0f
#define FOV_ZOOM 10.0f

void GLAPIENTRY debugMessageCallback(
        GLenum, GLenum type, GLuint, GLenum severity,
        GLsizei, const GLchar* message, const void*)
{
    Logger::warn << "Message [";
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR: Logger::warn << "other"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: Logger::warn << "performance"; break;
    case GL_DEBUG_TYPE_PORTABILITY: Logger::warn << "portability"; break;
    case GL_DEBUG_TYPE_POP_GROUP: Logger::warn << "pop group"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP: Logger::warn << "push group"; break;
    case GL_DEBUG_TYPE_MARKER: Logger::warn << "marker"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: Logger::warn << "undefined behavior"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: Logger::warn << "deprecated behavior"; break;
    default: Logger::warn << "unknown"; break;
    }
    Logger::warn << "] severity = 0x" << std::hex << severity <<
        " :\n\t" << message << std::dec << Logger::End;
}

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
    bool isVSyncActive = false;
#if USE_VSYNC
    if (SDL_GL_SetSwapInterval(1))
    {
        Logger::warn << "Failed to use V-Sync" << Logger::End;
        isVSyncActive = false;
    }
    else
    {
        Logger::verb << "Using V-Sync" << Logger::End;
        isVSyncActive = true;
    }
#endif
    Logger::verb << "Created and set up context" << Logger::End;

    glewExperimental = true;
    auto glewInitStatus = glewInit();
    if (glewInitStatus != GLEW_OK)
    {
        Logger::err << "Failed to initialize GLEW: " << glewGetErrorString(glewInitStatus) << Logger::End;
        return 1;
    }
    Logger::verb << "Initialized GLEW" << Logger::End;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debugMessageCallback, 0);

    Logger::log << "Using OpenGL " << glGetString(GL_VERSION) << Logger::End;


    ShaderProgram shader;
    if (shader.open("../shaders/basic.vert.glsl", "../shaders/basic.frag.glsl"))
        return 1;
    shader.use();

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);
    Camera camera{{0.0f, 1.0f, 0.0f}, (float)winW/winH};
    camera.setFovDeg(45.0f);
    camera.updateShaderUniforms(shader.getId());


    auto textRenderer = std::make_unique<OverlayRenderer>();
    if (textRenderer->open("../models/font", "../models/crosshair.obj"))
        return 1;


    std::vector<std::shared_ptr<Model>> models;
    for (size_t i{}; i < Assets::modelCount; ++i)
    {
        models.push_back(std::make_shared<Model>());
        if (models.back()->open(Assets::models[i]))
            return 1;
    }

    std::vector<std::shared_ptr<Texture>> textures;
    for (size_t i{}; i < Assets::textureCount; ++i)
    {
        textures.push_back(std::make_shared<Texture>());
        if (textures.back()->open(Assets::textures[i]))
            return 1;
    }

    std::vector<std::shared_ptr<GameObject>> gameObjects;
    gameObjects.emplace_back(std::make_shared<GameObject>(models[0], textures[0]));
    gameObjects.back()->translate({0.0f, 0.0f, -5.0f});
    gameObjects.emplace_back(std::make_shared<GameObject>(models[1], textures[1]));
    gameObjects.back()->translate({5.0f, 0.0f, 0.0f});
    gameObjects.emplace_back(std::make_shared<GameObject>(models[2], textures[2]));
    gameObjects.back()->translate({3.0f, 0.0f, 5.0f});

    glEnable(GL_DEPTH_TEST);

    uint32_t lastTime{};
    uint32_t deltaTime{};
    SDL_ShowCursor(false);
    bool shouldQuit = false;
    bool isInWireframeMode = false;
    bool isPaused = false;
    while (!shouldQuit)
    {
        uint32_t currentTime = SDL_GetTicks();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        int cursorX, cursorY;
        uint32_t mouseButtonState = SDL_GetMouseState(&cursorX, &cursorY);

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
                    camera.setWindowAspectRatio((float)event.window.data1/event.window.data2);
                    break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    shouldQuit = true;
                    break;

                case SDLK_TAB:
                    isPaused = !isPaused;
                    SDL_ShowCursor(isPaused);
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

        SDL_SetWindowTitle(window, (
                    "OpenGL Engine"
                    " - frame time = " + std::to_string(deltaTime) + "ms"
                    ", fps = " + std::to_string(int(1/(deltaTime/1000.0)))
                    + ", V-Sync " + (isVSyncActive ? "On" : "Off")).c_str());

        int windowW, windowH;
        SDL_GetWindowSize(window, &windowW, &windowH);

        // Cursor movement handling
        if (!isPaused)
        {
            int windowCenterX = windowW / 2;
            int windowCenterY = windowH / 2;

            if (cursorX != windowCenterX || cursorY != windowCenterY)
            {
                camera.setYawDeg(camera.getYawDeg() + (cursorX - windowCenterX) * MOUSE_SENS);
                camera.setPitchDeg(camera.getPitchDeg() + (windowCenterY - cursorY) * MOUSE_SENS);
                SDL_WarpMouseInWindow(window, windowCenterX, windowCenterY);
                camera.recalculateFrontVector();
            }
        }

        // Keypress handling
        {
            constexpr float cameraSpeed = 0.01f;
            auto keyboardState = SDL_GetKeyboardState(nullptr);
            if (keyboardState[SDL_SCANCODE_W])
                camera.moveForward(cameraSpeed, deltaTime);
            else if (keyboardState[SDL_SCANCODE_S])
                camera.moveBackwards(cameraSpeed, deltaTime);
            if (keyboardState[SDL_SCANCODE_A])
                camera.moveLeft(cameraSpeed, deltaTime);
            else if (keyboardState[SDL_SCANCODE_D])
                camera.moveRight(cameraSpeed, deltaTime);
        }

        // Zoom with the right mouse button
        if (mouseButtonState & SDL_BUTTON(SDL_BUTTON_RIGHT))
        {
            if (camera.getFovDeg() > FOV_ZOOM)
                camera.setFovDeg(camera.getFovDeg()-5.0f);
        }
        else
        {
            if (camera.getFovDeg() < FOV_DEFAULT)
                camera.setFovDeg(camera.getFovDeg()+5.0f);
        }

        glClearColor(0.34f, 0.406f, 0.642f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        camera.updateShaderUniforms(shader.getId());

        for (size_t i{}; i < gameObjects.size(); ++i)
            gameObjects[i]->draw(shader.getId());

        textRenderer->drawCrosshair((float)windowW/windowH);

        textRenderer->renderText(
                "FT: " + std::to_string(deltaTime) + "ms\n"
                + std::to_string(int(1/(deltaTime/1000.0))) + " FPS",
                0.1f, {1.70f*10, 0.0f});

        SDL_GL_SwapWindow(window);
    }


    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    Logger::verb << "Cleaned up" << Logger::End;
    return 0;
}

