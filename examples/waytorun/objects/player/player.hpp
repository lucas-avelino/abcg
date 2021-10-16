#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <chrono>

#include "../../types.hpp"
#include "abcg.hpp"

const float _scale = 0.125f;
const float initialPos = -0.5f * _scale;

class Player {
 public:
  std::array<float, 2> pos{initialPos, initialPos};
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

 private:
  // Input input{};

  GLuint renderLayer{};

  GLuint vao{};
  GLuint vbo{};
  GLuint ebo{};

  GLint translationLoc{};
  GLint colorLoc{};
  GLint scaleLoc{};
  GLint rotationLoc{};

  glm::vec4 color{1.0f, 0.0f, 0.0f, 1.0f};
  float rotation{};
  float scale{_scale};
  glm::vec2 translation{};
  // Time
  // int64_t timer{0};

  // MovementFunctions
  void leftKeyDown();
  void rightKeyDown();
  void down();

  void updatePosition();
  void startMovement(int coord, float aceleration);
  void startMovement(int coord, float aceleration, float velocity);

  const float baseAceleration{.5f};
  const float maxVelocity{1.0f};

  std::array<short, 2> previousMovementDirection{0, 0};
  std::array<bool, 2> hasMovement{false, false};
  std::array<bool, 2> isAcelerated{false, false};
  std::array<float, 2> S0{initialPos, initialPos};
  std::array<int64_t, 2> T0{0, 0};
  std::array<float, 2> V0{0, 0};
  std::array<float, 2> V{0, 0};
  std::array<float, 2> A{.0f, .0f};
};

#endif