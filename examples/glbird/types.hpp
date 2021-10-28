#ifndef COORD_HPP_
#define COORD_HPP_
#include "abcg.hpp"

struct Coord {
  float x;
  float y;
};

struct TubesColision {
  glm::vec2 bottomTubePosition;
  glm::vec2 topTubePosition;
};

struct ColisionBox {
  glm::vec2 firstCordinate;
  glm::vec2 secondCordinate;
};

struct ColisionCircle {
  glm::vec2 cordinate;
  float r;
};

#endif