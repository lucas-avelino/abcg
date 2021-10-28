project(glbird)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp input.cpp ./objects/player/player.cpp ./objects/pipes/pipes.cpp)

enable_abcg(${PROJECT_NAME})
