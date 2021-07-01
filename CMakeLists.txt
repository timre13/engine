CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED true)

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -g")

PROJECT(engine VERSION 1.0)

LINK_LIBRARIES(
    SDL2
    GL
    GLEW
)

ADD_EXECUTABLE(engine
    src/main.cpp
    src/Logger.cpp
    src/ShaderProgram.cpp
    src/GameObject.cpp
    src/Model.cpp
    src/Camera.cpp
    src/Texture.cpp
    src/TextRenderer.cpp
)

# Generate the font models on build
ADD_CUSTOM_TARGET(
    font_models
    COMMAND python3 split_font_model.py
    WORKING_DIRECTORY ../models
)
ADD_DEPENDENCIES(engine font_models)

