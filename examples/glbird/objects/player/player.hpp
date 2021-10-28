#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <chrono>

#include "../../types.hpp"
#include "abcg.hpp"

const float _scale = 0.125f * 0.4;
const float _initialPos = -0.5f * _scale;

class Player {
 public:
  std::array<float, 2> pos{_initialPos, _initialPos};
  void initializeGL(GLuint program);
  void paintGL(int8_t gameState);
  void terminateGL();
  void reset();
  ColisionCircle colisionCircle;

 private:

  GLuint renderLayer{};

  GLuint vao{};
  GLuint vbo{};
  GLuint ebo{};

  GLint translationLoc{};
  GLint colorLoc{};
  GLint scaleLoc{};
  GLint rotationLoc{};

  glm::vec4 color{1.0f, 0.6f, 0.33f, 1.0f}; //255, 155, 84
  float rotation{};
  float scale{_scale};
  glm::vec2 translation{};
  // Time
  int64_t timer{0};

  void updatePosition();
  void spaceKeyDown();
  void spaceKeyUp();

  bool isKeyPressed{false};

  float velocity{2.0f};
  float actualVelocity{0.0f};
  float movementInitialPosition{_initialPos};

  const float gravity{-4.0f};
};

#endif