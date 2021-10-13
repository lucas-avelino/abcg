#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include "abcg.hpp"
#include "./objects/player/player.hpp"
// #include "input.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  Player player;
  GLuint playerLayer{};
  // Input input{};

  int viewportWidth{};
  int viewportHeight{};

};

#endif