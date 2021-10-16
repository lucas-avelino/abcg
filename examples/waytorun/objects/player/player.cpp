#include "player.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <random>

#include "../../input.hpp"

extern Input globalInput;

void Player::leftKeyDown() {
  color = glm::vec4{0.0f, 0.5f, 1.0f, 1.0f};
  printf("[leftKeyDown] changeDirection, %f\n", A[0]);
  if (A[0] > 0 || V0[0] > 0 || !isAcelerated[0]) {
    startMovement(0, -baseAceleration);
  }
}

void Player::rightKeyDown() {
  color = glm::vec4{1.0f, 0.5f, 0.0f, 1.0f};
  if (A[0] < 0 || V0[0] < 0 || !isAcelerated[0]) {
    printf("[rightKeyDown] changeDirection\n");
    startMovement(0, baseAceleration);
  }
}

void Player::startMovement(int coord, float aceleration) {
  hasMovement[coord] = true;
  isAcelerated[coord] = true;
  T0[coord] = duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count();
  A[coord] = aceleration;
  V0[coord] = V[0];
  S0[coord] = pos[0];
}

void Player::startMovement(int coord, float aceleration, float velocity) {
  hasMovement[coord] = true;
  isAcelerated[coord] = true;
  T0[coord] = duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count();
  A[coord] = aceleration;
  V0[coord] = velocity;
  S0[coord] = pos[coord];
}

void Player::updatePosition() {
  if (hasMovement[0]) {
    int64_t t = duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count();

    float movementTime = (t - T0[0]) / 1000.0f;

    // Velocity

    V[0] = V0[0] + A[0] * (movementTime);
    if ((V[0] < 0 ? -V[0] : V[0]) > 0.2f) {
      V[0] = V[0] < 0 ? -0.2f : 0.2f;
      startMovement(0, 0, V[0]);
      previousMovementDirection[0] = 0;
      movementTime = (t - T0[0]) / 1000.0f;
    };

    short actualMovementDirection = V[0] < 0 ? -1 : V[0] != 0 ? 1 : 0;

    // Atrito
    if (V[0] > 0 && (actualMovementDirection != previousMovementDirection[0] ||
                     actualMovementDirection == 0)) {
      float friction = .5f * baseAceleration * actualMovementDirection;
      startMovement(0, friction + A[0]);
      movementTime = (t - T0[0]) / 1000.0f;
      previousMovementDirection[0] = actualMovementDirection;
    }

    // if (isXAcelerated)
    pos[0] = S0[0] + V0[0] * movementTime +
             ((A[0] * (movementTime * movementTime)) / 2);

    printf("time: %f \t", movementTime);
    printf("A[0]: %f \t", A[0]);
    printf("velocity: %f \t", V[0]);
    printf("velocity-V0[0]: %f \t", V0[0]);
    printf("pos: %f\n", pos[0]);
  }
  translation = glm::vec2{pos[0], pos[1]};
}

void Player::initializeGL(GLuint layerId) {
  terminateGL();

  globalInput.addListener(SDLK_LEFT, SDL_KEYDOWN,
                          std::bind(&Player::leftKeyDown, this));
  globalInput.addListener(SDLK_RIGHT, SDL_KEYDOWN,
                          std::bind(&Player::rightKeyDown, this));

  renderLayer = layerId;
  colorLoc = abcg::glGetUniformLocation(renderLayer, "color");
  scaleLoc = abcg::glGetUniformLocation(renderLayer, "scale");
  translationLoc = abcg::glGetUniformLocation(renderLayer, "translation");

  // clang-format off
  std::array<glm::vec2, 4> squareShape {
    glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f},
    glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f}
  };

  const std::array indices{0,1,2,
                           0,2,3};
  // clang-format on

  abcg::glGenBuffers(1, &vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(squareShape), squareShape.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glGenBuffers(1, &ebo);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  GLint positionAttribute{abcg::glGetAttribLocation(renderLayer, "inPosition")};

  abcg::glGenVertexArrays(1, &vao);
  abcg::glBindVertexArray(vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, vbo);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Player::paintGL() {
  updatePosition();
  abcg::glUseProgram(renderLayer);
  abcg::glBindVertexArray(vao);

  abcg::glUniform1f(scaleLoc, scale);
  abcg::glUniform2fv(translationLoc, 1, &translation.x);

  abcg::glUniform4fv(colorLoc, 1, &color.r);

  abcg::glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Player::terminateGL() {
  abcg::glDeleteBuffers(1, &vbo);
  abcg::glDeleteBuffers(1, &ebo);
  abcg::glDeleteVertexArrays(1, &vao);
}