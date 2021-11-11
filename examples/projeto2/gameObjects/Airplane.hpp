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

 private:
  GLuint VAO{};
  GLuint VBO{};
  GLuint EBO{};
  GLuint program{};

  // int viewportWidth{};
  // int viewportHeight{};

  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  void loadModelFromFile(std::string_view path);
};
#endif