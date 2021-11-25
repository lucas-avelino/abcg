project(airplaneObstacles)
add_executable(${PROJECT_NAME} camera.cpp ground.cpp main.cpp gameObjects/Airplane.cpp gameObjects/Buildings.cpp utils/input.cpp openglwindow.cpp)
enable_abcg(${PROJECT_NAME})
