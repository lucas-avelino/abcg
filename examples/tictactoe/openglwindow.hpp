#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <fmt/core.h>

#include <array>

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;

 private:
  std::array<std::array<int, 3>, 3> grid{
      {{-1, -1, -1}, {-1, -1, -1}, {-1, -1, -1}}};
      
  bool turn{false};

  int checkWinner(std::array<std::array<int, 3>, 3> grid) {
    for (int i = 0; i < 3; i++) {
      // horizontal check
      if (grid[i][0] == grid[i][1] && grid[i][0] == grid[i][2]) {
        return grid[i][0];
      }
      // vertical check
      if (grid[0][i] == grid[1][i] && grid[0][i] == grid[2][i]) {
        return grid[0][i];
      }
    }
    if (grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2]) return grid[0][0];
    if (grid[2][0] == grid[1][1] && grid[1][1] == grid[0][2]) return grid[2][0];
    return -1;
  };

  const char* turnName(bool turn) { return turn ? "0" : "X"; }
};

#endif