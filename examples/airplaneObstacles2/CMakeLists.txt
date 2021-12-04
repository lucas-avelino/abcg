project(airplaneObstacles2)
add_executable(${PROJECT_NAME} camera.cpp ground.cpp main.cpp gameObjects/Airplane.cpp gameObjects/Building.cpp utils/input.cpp utils/types.cpp openglwindow.cpp)
enable_abcg(${PROJECT_NAME})
