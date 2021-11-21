#ifndef AIRPLANE_HPP_
#define AIRPLANE_HPP_

#include <vector>

#include "abcg.hpp"

struct Vertex {
  glm::vec3 position;

  bool operator==(const Vertex& other) const {
    return position == other.position;
  }
};

class Airplane {
 public:
  void initializeGL(GLuint program, std::string assetsPath);
  void paintGL();
  void resizeGL(int width, int height);
  void terminateGL();
  glm::vec3 position{.0f, .5f, 0.0f};

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

  void loadModelFromFile(std::string_view path);
  void move();
};
#endif