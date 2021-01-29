cmake_minimum_required(VERSION 3.0)

project("hello-cg")

add_executable(${PROJECT_NAME}
    "src/Application.cpp"
    "src/Transform.cpp"
    "src/Utility.cpp"
    "src/main.cpp"
)

target_link_libraries(${PROJECT_NAME}
    "GL"
    "GLEW"
    "glfw"
)

