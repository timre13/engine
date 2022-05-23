#include "init.h"
#include "Logger.h"
#include <GL/glew.h>
#include <GL/gl.h>

#define INITIAL_WIN_W 1500
#define INITIAL_WIN_H 1000
//#define WINDOW_FULLSCREEN_MODE SDL_WINDOW_FULLSCREEN_DESKTOP
#define WINDOW_FULLSCREEN_MODE 0

static void GLAPIENTRY _debugMessageCallback(
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


namespace Init
{

int initVideo(SDL_Window** winOut, SDL_GLContext* contOut, bool* isVSyncActiveOut)
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        Logger::err << "Failed to initialize SDL: " << SDL_GetError() << Logger::End;
        return 1;
    }
    Logger::verb << "Initialized SDL" << Logger::End;

    *winOut = SDL_CreateWindow(
            "OpenGL Engine",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            INITIAL_WIN_W, INITIAL_WIN_H,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | (WINDOW_FULLSCREEN_MODE ? WINDOW_FULLSCREEN_MODE : 0)
    );
    if (!*winOut)
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
    *contOut = SDL_GL_CreateContext(*winOut);
    if (!*contOut)
    {
        Logger::err << "Failed to create OpenGL context: " << SDL_GetError() << Logger::End;
        return 1;
    }
    *isVSyncActiveOut = false;
#if USE_VSYNC
    if (SDL_GL_SetSwapInterval(1))
    {
        Logger::warn << "Failed to use V-Sync" << Logger::End;
        *isVSyncActiveOut = false;
    }
    else
    {
        Logger::verb << "Using V-Sync" << Logger::End;
        *isVSyncActiveOut = true;
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
    SDL_GL_SwapWindow(*winOut);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(_debugMessageCallback, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Logger::log << "Using OpenGL " << glGetString(GL_VERSION) << Logger::End;

    return 0;
}

} // namespace Init
