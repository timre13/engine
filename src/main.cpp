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
#include "ui/Window.h"
#include "ui/Button.h"
#include "ui/colors.h"

#define INITIAL_WIN_W 1500
#define INITIAL_WIN_H 1000
//#define WINDOW_FULLSCREEN_MODE SDL_WINDOW_FULLSCREEN_DESKTOP
#define WINDOW_FULLSCREEN_MODE 0
#define MOUSE_SENS 0.1f
#define USE_VSYNC 1
#define FOV_DEFAULT 45.0f
#define FOV_ZOOM 10.0f

void GLAPIENTRY debugMessageCallback(
        GLenum source, GLenum type, GLuint, GLenum severity,
        GLsizei, const GLchar* message, const void*)
{
    Logger::warn << "Message: type=\"";
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

    Logger::warn << "\", source=\"";
    switch (source)
    {
    case GL_DEBUG_SOURCE_API: Logger::warn << "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: Logger::warn << "window system"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: Logger::warn << "shader compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: Logger::warn << "third party"; break;
    case GL_DEBUG_SOURCE_APPLICATION: Logger::warn << "API"; break;
    case GL_DEBUG_SOURCE_OTHER: Logger::warn << "API"; break;
    default: Logger::warn << "unknown"; break;
    }

    Logger::warn << "\", severity=\"";
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH: Logger::warn << "high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: Logger::warn << "medium"; break;
    case GL_DEBUG_SEVERITY_LOW: Logger::warn << "low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: Logger::warn << "notification"; break;
    default: Logger::warn << "unknown"; break;
    }

    Logger::warn << "\" : " << message << Logger::End;
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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


    auto overlayRenderer = std::make_shared<UI::OverlayRenderer>();
    if (overlayRenderer->construct("../models/crosshair.obj"))
        return 1;
    auto buildMenuWindow = std::make_unique<UI::Window>(overlayRenderer);
    static constexpr float buildMenuWidth = 80;
    static constexpr float buildMenuHeight = 50;
    static constexpr float buildMenuXPos = 10;
    static constexpr float buildMenuYPos = 5;
    buildMenuWindow->setSize({buildMenuWidth, buildMenuHeight});
    buildMenuWindow->setPos({buildMenuXPos, buildMenuYPos});
    buildMenuWindow->setBgColor(UI_COLOR_BG);
    {
        static constexpr int numOfCols = 8;
        static constexpr float thingRectSpacing = 1.7f;
        static constexpr float menuBorder = 2.0f;
        static constexpr float thingRectSize = 8.0f;

        //textures[0]->bind();

        // XXX: Use orthographic projection to draw the things on the UI

        for (size_t i{}; i < models.size(); ++i)
        {
            auto button = std::make_shared<UI::Button>();
            button->setSize({thingRectSize, thingRectSize});
            button->setPos({
                    buildMenuXPos+menuBorder+i%numOfCols*(thingRectSize+thingRectSpacing),
                    buildMenuYPos+buildMenuHeight-menuBorder-thingRectSize-int(i/numOfCols)*(thingRectSize+thingRectSpacing)
            });
            button->setBgColor(UI_COLOR_FG);
            button->setHoveredBgColor(UI_COLOR_FG_HOVERED);
            button->setMouseClickCallback([](UI::Widget*, float, float){Logger::log << "Clicked" << Logger::End;});
            buildMenuWindow->addChild(button);
        }
    }



    //---------------------------- testing ----------------------------
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
    bool isBuildMenuShown = false;
    glm::vec<2, int> currCursorPos{};
    glm::vec<2, int> prevCursorPos{};
    while (!shouldQuit)
    {
        uint32_t currentTime = SDL_GetTicks();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        prevCursorPos.x = currCursorPos.x;
        prevCursorPos.y = currCursorPos.y;

        uint32_t mouseButtonState = SDL_GetMouseState(&currCursorPos.x, &currCursorPos.y);
        bool isLeftMouseButtonBeingReleased{};

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
                    overlayRenderer->setWindowSize(event.window.data1, event.window.data2);
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
                    SDL_ShowCursor(isBuildMenuShown || isPaused);
                    break;

                case SDLK_F3:
                    isInWireframeMode = !isInWireframeMode;
                    glPolygonMode(GL_FRONT_AND_BACK, isInWireframeMode ? GL_LINE : GL_FILL);
                    break;

                case SDLK_e:
                    isBuildMenuShown = !isBuildMenuShown;
                    SDL_ShowCursor(isBuildMenuShown || isPaused);
                    break;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    isLeftMouseButtonBeingReleased = true;
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
        if (!isPaused && !isBuildMenuShown)
        {
            const int windowCenterX = windowW / 2;
            const int windowCenterY = windowH / 2;

            if (currCursorPos.x != windowCenterX || currCursorPos.x != windowCenterY)
            {
                camera.setYawDeg(camera.getYawDeg() + (currCursorPos.x - windowCenterX) * MOUSE_SENS);
                camera.setPitchDeg(camera.getPitchDeg() + (windowCenterY - currCursorPos.y) * MOUSE_SENS);
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

        if (isBuildMenuShown)
        {
            const float windowRatio = (float)windowW/windowH;
            const float normCursorX = (float)currCursorPos.x/windowW*100;
            const float normCursorY = (100-(float)currCursorPos.y/windowH*100)/windowRatio;

            if (prevCursorPos.x != currCursorPos.x || prevCursorPos.y != currCursorPos.y)
                buildMenuWindow->onCursorMove(normCursorX, normCursorY);

            if (isLeftMouseButtonBeingReleased)
                buildMenuWindow->onMouseClick(normCursorX, normCursorY);

            buildMenuWindow->draw();
        }
        else
        {
            overlayRenderer->drawCrosshair();
        }

        std::string fpsText = "FT: " + std::to_string(deltaTime) + "ms\n"
            + std::to_string(int(1/(deltaTime/1000.0))) + " FPS";
        overlayRenderer->renderText(
                fpsText,
                1.0f,
                {windowW-DEF_FONT_SIZE*8, windowH-DEF_FONT_SIZE*2});

        overlayRenderer->commit();

        SDL_GL_SwapWindow(window);
    }


    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    Logger::verb << "Cleaned up" << Logger::End;
    return 0;
}

