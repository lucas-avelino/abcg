#ifndef BUILDING_HPP_
#define BUILDING_HPP_

#include <vector>

#include "../utils/types.hpp"
#include "abcg.hpp"

class Building {
 public:
  Building(glm::vec3 position){
    this->position = position;
  };
  void initializeGL(GLuint program, std::string assetsPath);
  void paintGL();
  void resizeGL(int width, int height);
  void terminateGL();
  glm::vec3 position{.0f, .5f, 0.0f};

 private:
  std::vector<GLuint> VAO{};
  std::vector<GLuint> VBO{};
  std::vector<GLuint> EBO{};
  GLuint program{};

  glm::vec3 rotate{0.0f, 1.0f, .0f};

  std::vector<std::vector<Vertex>> verticeGroups;
  std::vector<std::vector<GLuint>> indices;
  int64_t zeroTime{0};
  std::vector<glm::vec3> min;
  std::vector<glm::vec3> max;
  std::vector<glm::vec3> airplaneSize;

  std::vector<GLuint> m_diffuseTexture{};
  GLint rederingTypeLocale{};

  void loadModelFromFile(std::string_view path, std::string texturePath);
  void loadDiffuseTexture(std::string_view path, int index);

  void createBuffers();
  void setupVAO(int groupOffset);
  void setLightConfig();
  void computeNormals();

  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  std::vector<glm::vec4> m_Ka;
  std::vector<glm::vec4> m_Kd;
  std::vector<glm::vec4> m_Ks;
  std::vector<float> m_shininess{};

  bool m_hasNormals{false};
  bool m_hasTexCoords{false};
};
#endif