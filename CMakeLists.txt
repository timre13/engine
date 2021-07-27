CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED true)

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -g")

PROJECT(engine VERSION 1.0)

INCLUDE_DIRECTORIES(
    /usr/include/freetype2
    /usr/include/libpng16
)

LINK_LIBRARIES(
    SDL2
    GL
    GLEW
    freetype
)

ADD_EXECUTABLE(engine
    src/main.cpp
    src/Logger.cpp
    src/ShaderProgram.cpp
    src/GameObject.cpp
    src/Model.cpp
    src/Camera.cpp
    src/Texture.cpp
    src/ui/OverlayRenderer.cpp
    src/ui/Widget.cpp
    src/ui/Container.cpp
    src/ui/Window.cpp
    src/ui/Button.cpp
    src/os.cpp
)

