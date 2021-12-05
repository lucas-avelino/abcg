#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <list>
#include <vector>

#include "abcg.hpp"
#include "camera.hpp"
#include "gameObjects/airplane.hpp"
#include "gameObjects/building.hpp"
#include "ground.hpp"
#include "utils/types.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  // GLuint m_VAO{};
  // GLuint m_VBO{};
  // GLuint m_EBO{};
  GLuint solidColorProgram{};
  GLuint textureProgram{};

  int m_viewportWidth{};
  int m_viewportHeight{};
  ImFont* font{};

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};

  Ground m_ground;
  Airplane airplane;
  std::list<BuildingDuple> buildings{};
  void update();

  // Building spawn logic
  void respawnBuildings();
  void resetBuildings();

  // GameState
  GameState gameState{.state = 0};

  void spacePress();

  // Timer
  int64_t zeroTime{0};

  // Light
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka;
  glm::vec4 m_Kd;
  glm::vec4 m_Ks;
  float m_shininess{300.0f};
  glm::vec4 lighDir;
};

#endif