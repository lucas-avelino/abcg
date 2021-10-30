#include "openglwindow.hpp"

#include <imgui.h>

#include "abcg.hpp"
#include "input.hpp"

extern Input globalInput;

bool checkTubeColision(TubesColision tubeColision,
                       ColisionCircle colisionCircle) {
  float bottomTubeMaxY =
      tubeColision.bottomTubePosition[1] +
      (PipesContants::TUBE_TOTAL_HEIGHT * PipesContants::PIPES_SCALE);

  float topTubeMinY = tubeColision.bottomTubePosition[1] +
                      ((PipesContants::TUBE_TOTAL_HEIGHT +
                        PipesContants::SPACE_BETWEEN_TUBES + 1) *
                       PipesContants::PIPES_SCALE);
  float tubeMinX = tubeColision.bottomTubePosition[0];
  float tubeMaxX =
      tubeColision.bottomTubePosition[0] +
      (PipesContants::TUBE_TOTAL_WIDTH * PipesContants::PIPES_SCALE);

  return tubeMinX < colisionCircle.cordinate[0] + colisionCircle.r &&
         tubeMaxX > colisionCircle.cordinate[0] - colisionCircle.r &&
         (colisionCircle.cordinate[1] - bottomTubeMaxY <= colisionCircle.r ||
          colisionCircle.cordinate[1] + colisionCircle.r >= topTubeMinY);
}

void OpenGLWindow::handleEvent(SDL_Event& event) {
  globalInput.handleEvent(event);
}

void OpenGLWindow::addPoint() {
  points += 1;
}

void OpenGLWindow::initializeGame() {
  if (gameState != 1) {
    int64_t t = duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count();
    for (int i = 0; i < 2; i++) {
      pipes[i].timer = t + i * 1000;
      pipes[i].registryAddPointFunction(
          std::bind(&OpenGLWindow::addPoint, this));
      pipes[i].reset();
    }

    player.reset();
    gameState = 1;
  }
}

void OpenGLWindow::initializeGL() {
  ImGuiIO& io{ImGui::GetIO()};
  const auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Listeners
  globalInput.addListener(SDLK_SPACE, SDL_KEYDOWN,
                          std::bind(&OpenGLWindow::initializeGame, this));
  // Initialize pipes
  int64_t t = duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count();
  Pipes pipe1{1, t};
  Pipes pipe2{2, t + 1000};

  pipes = {pipe1, pipe2};
  for (int i = 0; i < 2; i++) {
    pipeLayers[i] = createProgramFromFile(getAssetsPath() + "pipes.vert",
                                          getAssetsPath() + "pipes.frag");
  }

  playerLayer = createProgramFromFile(getAssetsPath() + "objects.vert",
                                      getAssetsPath() + "objects.frag");
  abcg::glClearColor(0.36f, 0.98f, 1, 1);
#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif
  for (int i = 0; i < 2; i++) {
    pipes[i].initializeGL(pipeLayers[i]);
  }
  player.initializeGL(playerLayer);
}

void OpenGLWindow::paintGL() {
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, viewportWidth, viewportHeight);

  player.paintGL(gameState);
  for (int i = 0; i < 2; i++) {
    pipes[i].paintGL(gameState);
  }
  if (gameState == 1) {
    for (int i = 0; i < 2; i++) {
      if (checkTubeColision(pipes[i].tubeColision, player.colisionCircle)) {
        gameState = 2;
        points = 0;
      }
    }
  }
  if (player.pos[1] > 1) {
    gameState = 2;
    points = 0;
  } else if (player.pos[1] <= -1) {
    gameState = 2;
    points = 0;
  }
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
  {
    const auto size{ImVec2(viewportWidth, viewportHeight)};

    const auto position{ImVec2((size.x / 2) - 5, 0.0f)};

    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin(" ", nullptr,
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoInputs);
    ImGui::PushFont(font);
    // format_str = "%s";
    ImGui::Text("%i", points);

    ImGui::PopFont();
    ImGui::End();
    if (gameState == 2) {
      ImGui::SetNextWindowPos(ImVec2{(size.x / 2) - 150, (size.y / 2) - 50});
      ImGui::SetNextWindowSize(ImVec2(400, 100));
      ImGui::Begin("  ", nullptr,
                   ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoInputs);
      ImGui::PushFont(font);
      // format_str = "%s";
      ImGui::Text("Você perdeu");

      ImGui::PopFont();
      ImGui::End();
    }
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  viewportWidth = width;
  viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  for (int i = 0; i < 2; i++) {
    abcg::glDeleteProgram(pipeLayers[i]);
    pipes[i].terminateGL();
  }

  abcg::glDeleteProgram(playerLayer);

  player.terminateGL();
}
