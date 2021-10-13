#include "openglwindow.hpp"

#include <imgui.h>

#include "abcg.hpp"
#include "input.hpp"

extern Input globalInput;

void OpenGLWindow::handleEvent(SDL_Event& event) {
  globalInput.handleEvent(event);
}

void OpenGLWindow::initializeGL() {

  playerLayer = createProgramFromFile(getAssetsPath() + "objects.vert",
                                      getAssetsPath() + "objects.frag");
  abcg::glClearColor(0, 0, 0, 1);
#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif
  player.initializeGL(playerLayer);
}

void OpenGLWindow::paintGL() {
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, viewportWidth, viewportHeight);

  player.paintGL();
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
  {
    const auto size{ImVec2(300, 85)};
    const auto position{ImVec2((viewportWidth - size.x) / 2.0f,
                               (viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    // ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  viewportWidth = width;
  viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(playerLayer);

  player.terminateGL();
}

