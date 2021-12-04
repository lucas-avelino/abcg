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

bool reactsCollision(Rectangle r1, Rectangle r2) {
  // are the sides of one rectangle touching the other?

  if (r1.coord.x + r1.size.x >= r2.coord.x &&  // r1 right edge past r2 left
      r1.coord.x <= r2.coord.x + r2.size.x &&  // r1 left edge past r2 right
      r1.coord.y + r1.size.y >= r2.coord.y &&  // r1 top edge past r2 bottom
      r1.coord.y <= r2.coord.y + r2.size.y) {  // r1 bottom edge past r2 top
    return true;
  }
  return false;
}

void OpenGLWindow::respawnBuildings() {
  int newXPositionFirstItem = (rand() % 3) - 1;
  int newXPositionSecondItem = (rand() % 3) - 1;
  while (newXPositionFirstItem == newXPositionSecondItem)
    newXPositionSecondItem = (rand() % 3) - 1;

  BuildingDuple building = buildings.front();
  buildings.pop_front();

  building.building1.position.x = newXPositionFirstItem * 1.15f;
  building.building1.position.z = building.building1.position.z - 30.0f;
  building.building2.position.x = newXPositionSecondItem * 1.15f;
  building.building2.position.z = building.building2.position.z - 30.0f;
  buildings.push_back(building);
}

void OpenGLWindow::resetBuildings() {
  int i = 0;
  for (BuildingDuple& building : buildings) {
    int newXPositionFirstItem = (rand() % 3) - 1;
    int newXPositionSecondItem = (rand() % 3) - 1;
    while (newXPositionFirstItem == newXPositionSecondItem)
      newXPositionSecondItem = (rand() % 3) - 1;

    building.building1.initGame(
        glm::vec3(newXPositionFirstItem * 1.15f, 0, (-10.0f * i)));
    building.building2.initGame(
        glm::vec3(newXPositionSecondItem * 1.15f, 0, (-10.0f * i)));
    i++;
  }
}

void OpenGLWindow::spacePress() {
  if (gameState.state == 0) {
    resetBuildings();
    airplane.initGame();
    gameState.state = 1;
    zeroTime = duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();
  }
  if (gameState.state == 2) {
    gameState.state = 0;
     resetBuildings();
    airplane.initGame();
    gameState.state = 1;

  }
}

void OpenGLWindow::initializeGL() {
  ImGuiIO& io{ImGui::GetIO()};
  const auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

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

  buildings.push_back(
      BuildingDuple{.building1 = Building{glm::vec3(0, 0, .0f)},
                    .building2 = Building{glm::vec3(1.15f, 0.0f, .0f)}});
  buildings.push_back(
      BuildingDuple{.building1 = Building{glm::vec3(-1.15f, 0, -10.0f)},
                    .building2 = Building{glm::vec3(1.15f, 0, -10.0f)}});
  buildings.push_back(
      BuildingDuple{.building1 = Building{glm::vec3(-1.15f, 0, -20.0f)},
                    .building2 = Building{glm::vec3(0, 0, -20.0f)}});

  for (BuildingDuple& _building : buildings) {
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

  fmt::print("--> ({}, {}) \n", airplane.colisionRect.toString(),
             buildings.front().building1.colisionRect.toString());

  int airplaneZPositionFloor = floor(airplane.position.z);

  // Only check colision with airplane is near to 10 multiples
  if (gameState.state == 1  && airplaneZPositionFloor < 1 &&
      (airplaneZPositionFloor % 9 == 0 || airplaneZPositionFloor % 10 == 0 ||
       airplaneZPositionFloor % 11 == 0)) {
    if (reactsCollision(buildings.front().building1.colisionRect,
                        airplane.colisionRect) ||
        reactsCollision(buildings.front().building2.colisionRect,
                        airplane.colisionRect)) {
      gameState.state = 2;
      fmt::print("Collision \n");
    }
  }

  if (buildings.front().building1.position.z > airplane.position.z + 3)
    respawnBuildings();

  for (BuildingDuple& _building : buildings) {
    _building.paintGL();
  }
  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
  {
    const auto size{ImVec2(m_viewportWidth, m_viewportHeight)};

    const auto position{ImVec2((size.x / 2) - 5, 0.0f)};

    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    if (gameState.state == 2) {
      ImGui::SetNextWindowPos(ImVec2{(size.x / 2) - 150, (size.y / 2) - 50});
      ImGui::SetNextWindowSize(ImVec2(400, 100));
      ImGui::Begin("  ", nullptr,
                   ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoInputs);
      ImGui::PushFont(this->font);
      ImGui::Text("Você perdeu");

      ImGui::PopFont();
      ImGui::End();
    }
  }
}

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

// bool reactsCollision(float r1x, float r1y, float r1w, float r1h, float r2x,
// float r2y, float r2w, float r2h)
