#ifndef VERTEX_HPP_
#define VERTEX_HPP_

struct Vertex {
  glm::vec3 position;

  bool operator==(const Vertex& other) const {
    return position == other.position;
  }
};

#endif