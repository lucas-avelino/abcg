#ifndef UTILS_TYPES_HPP_
#define UTILS_TYPES_HPP_
#include <fmt/core.h>

#include <glm/gtx/hash.hpp>

struct Vertex {
  glm::vec3 position{};
  glm::vec3 normal{};
  glm::vec2 texCoord{};

  bool operator==(const Vertex& other) const noexcept {
    static const auto epsilon{std::numeric_limits<float>::epsilon()};
    return glm::all(glm::epsilonEqual(position, other.position, epsilon)) &&
           glm::all(glm::epsilonEqual(normal, other.normal, epsilon)) &&
           glm::all(glm::epsilonEqual(texCoord, other.texCoord, epsilon));
  }
};

struct GameState {
  /**
   * 0 = To init
   * 1 = On game
   * 2 = Lose game state
   **/
  int state{0};
  int points{0};
};

struct Rectangle {
  glm::vec2 coord;
  glm::vec2 size;

  std::string toString() {
    return fmt::format("Coord: ({}, {}) Size: ({}, {})", coord.x, coord.y,
                       size.x, size.y);
  }
};

struct LightProperties {
  glm::vec4 Ia;
  glm::vec4 Id;
  glm::vec4 Is;
  float shininess;
  glm::vec4 lighDir;
};

#endif