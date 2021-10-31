#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include "abcg.hpp"
#include "./objects/player/player.hpp"
#include "./objects/pipes/pipes.hpp"
// #include "input.hpp"

//Colors
/**
 * sky: 0.36f, 0.98f , 1
*/

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  void initializeGame();
  Player player;
  GLuint playerLayer{};
  std::array<Pipes, 2> pipes{}; 
  std::array<GLuint, 2> pipeLayers{}; 


  int viewportWidth{800};
  int viewportHeight{800};
  int8_t gameState{0}; // 0 = stoped, 1 = inGame, 2 = lost
  ImFont* font{};
  int points{0};
  void addPoint();
};

#endif