#ifndef UTILS_TYPES_HPP_
#define UTILS_TYPES_HPP_
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

struct GameState{
  /**
   * 0 = To init
   * 1 = On game
   * 2 = Lose game state
  **/
  int state{0};
};

#endif