#ifndef BUILDINGS_HPP_
#define BUILDINGS_HPP_

#include <vector>

#include "abcg.hpp"
#include "../utils/types.hpp"

class Buildings {
 public:
  void initializeGL(GLuint program, std::string assetsPath);
  void paintGL();
  void resizeGL(int width, int height);
  void terminateGL();

 private:
  glm::vec3 position{.0f, .0f, -5.0f};
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

  void loadModelFromFile(std::string_view path);
};
#endif