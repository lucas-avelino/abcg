#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  // abcg::glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
  //                    m_clearColor[3]);
  // Clear the color buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
  abcg::OpenGLWindow::paintUI();

  {

    ImGui::Text("Turn %s", turnName(turn));
    int winner = checkWinner(grid);
    if (winner != -1) {
      ImGui::Text("The winner is: %s", turnName(winner == 0));
    }

    // render buttons 
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        ImGui::Button(grid[i][j] == -1 ? "" : turnName(grid[i][j] == 0),
                      ImVec2(100, 100));
        if (ImGui::IsItemClicked() && grid[i][j] == -1 && winner == -1) {
          grid[i][j] = turn ? 0 : 1;
          turn = !turn;
        }
        if (j % 3 != 2) {
          ImGui::SameLine();
        }else{
          ImGui::Separator();
        }
      }
    }
    ImGui::Button("Reset", ImVec2(-1, 50));
    if (ImGui::IsItemClicked()) {
      grid = { {{-1, -1, -1}, {-1, -1, -1}, {-1, -1, -1}} };
    }
  }
}
