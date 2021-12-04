#ifndef AIRPLANE_HPP_
#define AIRPLANE_HPP_

#include <vector>

#include "../utils/types.hpp"
#include "abcg.hpp"

class Airplane {
 public:
  void initializeGL(GLuint program, std::string assetsPath);
  void paintGL(GameState gameSate);
  void resizeGL(int width, int height);
  void terminateGL();
  void initGame();
  glm::vec3 position{.0f, 1.0f, 0.0f};

 private:
  GLuint VAO{};
  GLuint VBO{};
  GLuint EBO{};
  GLuint program{};

  glm::vec3 rotate{0.0f, 1.0f, .0f};

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  int64_t zeroTime{0};
  glm::vec3 min;
  glm::vec3 max;
  glm::vec3 airplaneSize;

  GLuint m_diffuseTexture{};
  GLint rederingTypeLocale{};

  void loadModelFromFile(std::string_view path, std::string texturePath);
  void loadDiffuseTexture(std::string_view path);

  void move();
  void createBuffers();
  void setupVAO();
  void setLightConfig();
  void computeNormals();

  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka;
  glm::vec4 m_Kd;
  glm::vec4 m_Ks;
  float m_shininess{};
  bool m_hasNormals{false};
  bool m_hasTexCoords{false};

  //Controlls
  void bindControlls();
  void left();
  void rigth();
  int targetPosition{0};
  float initialPosition{0};
  float actualPosition{0};
  float curveVelocity{0.003f};
  int64_t movementStart{0};
  const float positionModifier{1.25f};
  const float curveVelocitybase{0.003f};

  //Forward Movement
  float aceleration{-0.25f};
  float velocity{-2.0f};
  float forwardInitialPosition{30.0f};
};
#endif