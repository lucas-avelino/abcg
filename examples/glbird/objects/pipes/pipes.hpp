#ifndef PIPES_HPP_
#define PIPES_HPP_

#include <chrono>
#include <functional>

#include "../../types.hpp"
#include "abcg.hpp"

const float _Pipes_scale = 0.125f;
const float _Pipes_initialPos = -1.0f * _Pipes_scale;

namespace PipesContants{
  static const float PIPES_SCALE = 0.125f;
  static const float TUBE_TOTAL_HEIGHT = 6.0f;
  static const float TUBE_TOTAL_WIDTH = 4.0f;
  static const float SPACE_BETWEEN_TUBES = 8.0f;
}

class Pipes {
 public:
  Pipes();
  Pipes(int _id, int64_t timer);
  Pipes(int _id, int64_t timer, glm::vec4 color);
  void initializeGL(GLuint program);
  void paintGL(int8_t gameState);
  void terminateGL();
  void reset();
  std::array<float, 2> pos{_Pipes_initialPos, -10.5f * _Pipes_scale};
  int64_t timer{0};
  void registryAddPointFunction(std::function<void()> f);
  int id;
  TubesColision tubeColision;
  ColisionBox colisionBox;

 private:
  void randomizeHeight();
  GLuint renderLayer{};

  GLuint vao{};
  GLuint vbo{};
  GLuint ebo{};

  GLint translationLoc{};
  GLint colorLoc{};
  GLint scaleLoc{};
  GLint rotationLoc{};

  glm::vec4 color{0.0f, 1.0f, 0.0f, 1.0f};
  float rotation{};
  float scale{_Pipes_scale};
  glm::vec2 translation{};
  void updatePosition();

  float INITIAL_POSTION{2.0f};
  float velocity{-2.0f};
  bool pointWasCount{false};

  // Ref
  std::function<void()> addPoint;

  float spaceBetweenTubes = 8.0f;
  float tubeHeight = 6.0f;
  float tubeWidth = 2.0f;
};

#endif