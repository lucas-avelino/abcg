#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <vector>

#include "abcg.hpp"
#include "camera.hpp"
#include "gameObjects/Airplane.hpp"
#include "gameObjects/Buildings.hpp"
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
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};
  std::vector<GLuint> m_programs;
  std::vector<const char*> m_shaderNames{"texture", "blinnphong", "phong",
                                         "gouraud", "normal",     "depth"};

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};
  int m_mappingMode{};
  int m_trianglesToDraw{};
  int m_currentProgramIndex{};

  Ground m_ground;
  Airplane airplane;
  Buildings buildings;

  // Light and material properties
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka;
  glm::vec4 m_Kd;
  glm::vec4 m_Ks;
  float m_shininess{};
  
  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};

  void loadModel(std::string_view path);

  void update();

  
};

#endif