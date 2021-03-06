#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <filesystem>
#include <functional>
#include "init.h"
#include "Logger.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "GameObject.h"
#include "assets.h"
#include "Model.h"
#include "Texture.h"
#include "GameMap.h"
#include "ui/Window.h"
#include "ui/Button.h"
#include "ui/colors.h"
#include "PhysicsWorld.h"
#include "FileCache.h"

#define MOUSE_SENS 0.1f
#define USE_VSYNC 1
#define FOV_DEFAULT 45.0f
#define FOV_ZOOM 10.0f

/*
static UI::Window* createBuildMenuWin(std::shared_ptr<UI::OverlayRenderer> olrend, size_t modelCount)
{
    auto win = new UI::Window{olrend};
    static constexpr float buildMenuWidth = 80;
    static constexpr float buildMenuHeight = 50;
    static constexpr float buildMenuXPos = 10;
    static constexpr float buildMenuYPos = 5;
    win->setSize({buildMenuWidth, buildMenuHeight});
    win->setPos({buildMenuXPos, buildMenuYPos});
    {
        static constexpr int numOfCols = 8;
        static constexpr float thingRectSpacing = 1.7f;
        static constexpr float menuBorder = 2.0f;
        static constexpr float thingRectSize = 8.0f;

        // XXX: Draw buildable stuff images

        for (size_t i{}; i < modelCount; ++i)
        {
            auto button = std::make_shared<UI::Button>();
            button->setSize({thingRectSize, thingRectSize});
            button->setPos({
                    buildMenuXPos+menuBorder+i%numOfCols*(thingRectSize+thingRectSpacing),
                    buildMenuYPos+buildMenuHeight-menuBorder-thingRectSize-int(i/numOfCols)*(thingRectSize+thingRectSpacing)
            });
            button->setText(std::to_string(i));
            button->setMouseClickCallback([](UI::Widget* w, float, float){
                    Logger::log << "Clicked button with text \"" << dynamic_cast<UI::Button*>(w)->getText() << "\"" << Logger::End;
            });
            win->addChild(button);
        }
    }
    return win;
}
*/

int main()
{
    SDL_Window* window;
    SDL_GLContext context;
    bool isVSyncActive;
    if (Init::initVideo(&window, &context, &isVSyncActive))
    {
        Logger::err << "Exiting with error" << Logger::End;
        return 1;
    }

    ShaderProgram shader;
    if (shader.open("../shaders/basic.vert.glsl", "../shaders/basic.frag.glsl"))
        return 1;
    shader.use();

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);
    Camera camera{{0.0f, 10.0f, 0.0f}, (float)winW/winH};
    camera.setFovDeg(45.0f);
    camera.updateShaderUniforms(shader.getId());

    static constexpr auto modelAssetDir = ASSET_DIR_MODELS;
    static constexpr auto modelPlaceholderFilename = std::string_view{};
    FileCache<Model, modelAssetDir, modelPlaceholderFilename> modelCache;
    static constexpr auto textureAssetDir = ASSET_DIR_TEXTURES;
    static constexpr auto texturePlaceholderFilename = std::string_view{"placeholder.png"};
    FileCache<Texture, textureAssetDir, texturePlaceholderFilename> textureCache;

    auto overlayRenderer = std::make_shared<UI::OverlayRenderer>();
    if (overlayRenderer->construct("../assets/crosshair.obj"))
        return 1;
    //auto buildMenuWindow = std::unique_ptr<UI::Window>{createBuildMenuWin(overlayRenderer, models.size())};

    PhysicsWorld pworld;

    std::vector<std::unique_ptr<GameObject>> gameObjects;
    {
        GameMap map{"../maps/test.json"};
        // Note: `objdescr->collShape` will be cleared here
        for (const auto& objdescr : map.getObjects())
        {
            std::shared_ptr<Model> model = modelCache.open(objdescr->modelName);
            std::shared_ptr<Texture> texture = textureCache.open(objdescr->textureName);

            const glm::vec3 mRotRad = {
                glm::radians(objdescr->modelRotDeg.x),
                glm::radians(objdescr->modelRotDeg.y),
                glm::radians(objdescr->modelRotDeg.z)};

            gameObjects.push_back(std::unique_ptr<GameObject>{new GameObject{
                    model, mRotRad, texture, std::move(objdescr->collShape), objdescr->mass, objdescr->objName, objdescr->flags}});
            gameObjects.back()->setPos(objdescr->pos);
            gameObjects.back()->scale(objdescr->scale);
            pworld.addObject(gameObjects.back().get());
        }
    }

    bool isDbgMenuOpen = false;
    bool isWireframeMode = false;
    bool isBlendingOn = true;
    bool isFaceCullingOn = true;
    bool isMultisamplingOn = true;
    struct DbgMenuItem
    {
        std::string name;
        std::function<void()> callback;
        std::function<bool()> isOn;
    };
    const DbgMenuItem dbgMenuItems[]{
        {"Wireframe mode", [&](){
            isWireframeMode = !isWireframeMode;
            glPolygonMode(GL_FRONT_AND_BACK, isWireframeMode ? GL_LINE : GL_FILL);
        }, [&](){
            return isWireframeMode;
        }},
        {"Blending", [&](){
            isBlendingOn = !isBlendingOn;
            if (isBlendingOn) glEnable(GL_BLEND);
            else glDisable(GL_BLEND);
        }, [&](){
            return isBlendingOn;
        }},
        {"Face culling", [&](){
            isFaceCullingOn = !isFaceCullingOn;
            if (isFaceCullingOn) glEnable(GL_CULL_FACE);
            else glDisable(GL_CULL_FACE);
        }, [&](){
            return isFaceCullingOn;
        }},
        {"Multisampling", [&](){
            isMultisamplingOn = !isMultisamplingOn;
            if (isMultisamplingOn) glEnable(GL_MULTISAMPLE);
            else glDisable(GL_MULTISAMPLE);
        }, [&](){
            return isMultisamplingOn;
        }},
        {"Bullet: Wireframe mode", [&](){
            pworld.setDbgMode(pworld.getDbgMode() ^ btIDebugDraw::DBG_DrawWireframe);
        }, [&](){
            return pworld.getDbgMode() & btIDebugDraw::DBG_DrawWireframe;
        }},
        {"Bullet: Draw AABB", [&](){
            pworld.setDbgMode(pworld.getDbgMode() ^ btIDebugDraw::DBG_DrawAabb);
        }, [&](){
            return pworld.getDbgMode() & btIDebugDraw::DBG_DrawAabb;
        }},
        {"Bullet: No deactivation", [&](){
            pworld.setDbgMode(pworld.getDbgMode() ^ btIDebugDraw::DBG_NoDeactivation);
        }, [&](){
            return pworld.getDbgMode() & btIDebugDraw::DBG_NoDeactivation;
        }},
    };
    constexpr int DBG_MENU_ITEM_COUNT = sizeof(dbgMenuItems)/sizeof(dbgMenuItems[0]);
    static_assert(DBG_MENU_ITEM_COUNT <= 9); // Only implemented for number keys (0 excluded)

    uint32_t lastTime{};
    uint32_t deltaTime{};
    SDL_ShowCursor(false);
    bool shouldQuit = false;
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
        //bool isLeftMouseButtonBeingReleased{};

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
                    isDbgMenuOpen = !isDbgMenuOpen;
                    break;

                case SDLK_e:
                    isBuildMenuShown = !isBuildMenuShown;
                    SDL_ShowCursor(isBuildMenuShown || isPaused);
                    break;
                }

                // Handle number keys while the debug menu is open
                if (isDbgMenuOpen)
                {
                    const int index = event.key.keysym.sym-SDLK_1;
                    if (index >= 0 && index < DBG_MENU_ITEM_COUNT)
                        dbgMenuItems[index].callback();
                }
                break;

            case SDL_MOUSEBUTTONUP:
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    //isLeftMouseButtonBeingReleased = true;
                    break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:
                {
                    static constexpr float RAY_LEN = 10.0f;
                    const btVector3 fromVec = camera.getPositionBt();
                    const btVector3 toVec = camera.getFrontPosBt(RAY_LEN);
                    btCollisionWorld::ClosestRayResultCallback cb{fromVec, toVec};
                    pworld.getWorld()->rayTest(fromVec, toVec, cb);
                    if (cb.hasHit())
                    {
                        // Get the object that is colliding with the ray
                        btCollisionObject* obj = (btCollisionObject*)cb.m_collisionObject; // const -> nonconst cast?
                        assert(obj);
                        obj->activate(true);
                        auto rigidBody = btRigidBody::upcast(obj);
                        assert(rigidBody);
                        const btVector3 collPos = fromVec.lerp(toVec, cb.m_closestHitFraction);
                        static constexpr float PUSH_FORCE = 5.0f;
                        // Apply force at the point of the object where the camera is facing at
                        rigidBody->applyImpulse(
                                camera.getFrontVecBt()*PUSH_FORCE,
                                collPos-rigidBody->getWorldTransform().getOrigin());
                    }
                    break;
                }
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

            if (currCursorPos.x != windowCenterX || currCursorPos.y != windowCenterY)
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

        if (isDbgMenuOpen)
        {
            std::string str = "Debug options:";
            for (int i{}; i < DBG_MENU_ITEM_COUNT; ++i)
                str += std::string("\n")+char('1'+i)+": "+dbgMenuItems[i].name+(dbgMenuItems[i].isOn() ? ": ON" : ": OFF");
            overlayRenderer->renderTextAtPerc(str, 1.0f, {1.f, 53.f}, {1.0f, 1.0f, 0.0f});
        }

        // Update physics
        pworld.updateDbgDrawUniforms(camera); // TODO: Refactor debug drawing
        const uint32_t phyStepStart = SDL_GetTicks();
        pworld.stepSimulation(1/60.0f);
        const uint32_t phyStepDur = SDL_GetTicks()-phyStepStart;
        pworld.applyTransforms(gameObjects);

        size_t drawnVertices{};
        shader.use();
        camera.updateShaderUniforms(shader.getId());
        for (size_t i{}; i < gameObjects.size(); ++i)
            drawnVertices += gameObjects[i]->draw(shader.getId());

        /*
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
        */
        overlayRenderer->drawCrosshair();
        /*
        }
        */

        const std::string renderInfoText
            = "Frame time:   " + std::to_string(deltaTime) + "ms"
            + "\nPhysics time: " + std::to_string(phyStepDur) + "ms"
            + "\nFPS:          " + std::to_string(int(1/(deltaTime/1000.0)))
            + "\nObjs drawn:   " + std::to_string(gameObjects.size())
            + "\nVerts drawn:  " + std::to_string(drawnVertices);
        overlayRenderer->renderTextAtPx(renderInfoText, 1.0f,
                {windowW-DEF_FONT_SIZE*15, windowH-DEF_FONT_SIZE*2});

        overlayRenderer->commit();

        SDL_GL_SwapWindow(window);
    }


    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    Logger::verb << "Cleaned up" << Logger::End;
    return 0;
}

