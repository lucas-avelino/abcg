#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

#include "utils/input.hpp"

extern Input globalInput;

void OpenGLWindow::handleEvent(SDL_Event& event) {
  globalInput.handleEvent(event);
}

void OpenGLWindow::respawnBuildings(std::vector<int> toRespawnBuildings) {
  fmt::print("[OpenGLWindow][respawnBuildings] started\n");
  int newXPositionFirstItem = (rand() % 3) - 1;
  int newXPositionSecondItem = (rand() % 3) - 1;
  while (newXPositionFirstItem == newXPositionSecondItem)
    newXPositionSecondItem = (rand() % 3) - 1;

  buildings.at(toRespawnBuildings.at(0)).position.x =
      newXPositionFirstItem * 1.15f;
  buildings.at(toRespawnBuildings.at(0)).position.z =
      buildings.at(toRespawnBuildings.at(0)).position.z - 30.0f;
  buildings.at(toRespawnBuildings.at(1)).position.x =
      newXPositionSecondItem * 1.15f;
  buildings.at(toRespawnBuildings.at(1)).position.z =
      buildings.at(toRespawnBuildings.at(1)).position.z - 30.0f;
}

void OpenGLWindow::spacePress() {
  if (gameState.state == 0) {
    airplane.initGame();
    gameState.state = 1;
  }
}

void OpenGLWindow::initializeGL() {
  abcg::glClearColor(0.529f, 0.807f, 0.921f, 1);

  globalInput.addListener(SDLK_SPACE, SDL_KEYDOWN,
                          std::bind(&OpenGLWindow::spacePress, this));

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "lookat.vert",
                                    getAssetsPath() + "lookat.frag");

  m_ground.initializeGL(m_program);
  airplane.initializeGL(m_program, getAssetsPath());
  for (Building& _building : buildings) {
    fmt::print("Passou\n");
    _building.initializeGL(m_program, getAssetsPath());
  }
  resizeGL(getWindowSettings().width, getWindowSettings().height);
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_projMatrix[0][0]);
  const GLint normalMatrixLoc{
      abcg::glGetUniformLocation(m_program, "normalMatrix")};
  const auto modelViewMatrix{
      glm::mat3(m_camera.m_viewMatrix * m_camera.m_projMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};

  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  airplane.paintGL(gameState);
  m_ground.paintGL();

  std::vector<int> toRespawnIndexes{};
  for (int i : iter::range(buildings.size())) {
    if (buildings.at(i).position.z > airplane.position.z + 3)
      toRespawnIndexes.push_back(i);
  }

  if (!toRespawnIndexes.empty()) respawnBuildings(toRespawnIndexes);

  for (Building& _building : buildings) {
    _building.paintGL();
  }
  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() { abcg::OpenGLWindow::paintUI(); }

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  m_ground.terminateGL();
  airplane.terminateGL();
  for (auto _building : buildings) _building.terminateGL();
  abcg::glDeleteProgram(m_program);
}
void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);

  m_camera.follow(airplane.position);
}