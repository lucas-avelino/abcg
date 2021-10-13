#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "abcg.hpp"


class Player {
 public:
  float x{};
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
  float scale{0.25f};
  glm::vec2 translation{glm::vec2(-0.5f*0.25f)};

  //MovementFunctions
  void up();
  void down();
};

#endif