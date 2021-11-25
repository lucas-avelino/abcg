// Explicit specialization of std::hash for Vertex

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include "abcg.hpp"

namespace std {
  template <>
    struct hash<Vertex> {
      size_t operator()(Vertex const& vertex) const noexcept {
        const std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
        return h1;
      }
    };
} 