project(projeto2)
add_executable(${PROJECT_NAME} camera.cpp ground.cpp main.cpp gameObjects/Airplane.cpp utils/input.cpp openglwindow.cpp)
enable_abcg(${PROJECT_NAME})
