#ifndef BUILDING_HPP_
#define BUILDING_HPP_

#include <fmt/core.h>

#include <vector>

#include "../utils/types.hpp"
#include "abcg.hpp"

class Building {
 public:
  Building(glm::vec3 position) { this->position = position; };
  void initializeGL(GLuint program, std::string assetsPath);
  void paintGL(LightProperties light);
  void resizeGL(int width, int height);
  void terminateGL();
  void initGame(glm::vec3 position);
  glm::vec3 position{.0f, .5f, 0.0f};
  Rectangle colisionRect;
  std::string toString() {
    return fmt::format("[Building] position ({}, {}, {})", position.x,
                       position.y, position.z);
  };

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
  void setLightConfig(LightProperties light, int textureIndex);
  void computeNormals();

  std::vector<glm::vec4> m_Ka;
  std::vector<glm::vec4> m_Kd;
  std::vector<glm::vec4> m_Ks;
  std::vector<float> m_shininess{300.0f};

  bool m_hasNormals{false};
  bool m_hasTexCoords{false};

  // Scale
  const glm::vec3 scale{ glm::vec3(0.0048f) * glm::vec3(1, 1, 0.5) };
};

struct BuildingDuple {
  Building building1;
  Building building2;

  void paintGL(LightProperties light) {
    building1.paintGL(light);
    building2.paintGL(light);
  };

  void initializeGL(GLuint program, std::string assetsPath) {
    building1.initializeGL(program, assetsPath);
    building2.initializeGL(program, assetsPath);
  };

  void terminateGL() {
    building1.terminateGL();
    building2.terminateGL();
  };
};

#endif