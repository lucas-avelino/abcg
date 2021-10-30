#include "player.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <random>

#include "../../input.hpp"

extern Input globalInput;

void Player::updatePosition() {
  int64_t actualTime = duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();
  float t = (actualTime - timer) * .002;
  // actualVelocity = velocity + gravity * t;
  pos[1] = movementInitialPosition + t * velocity + ((gravity) * (t * t)) / 2;

  if (pos[1] < -1) pos[1] = -1;  // parar no chão
  translation = glm::vec2{pos[0], pos[1]};

  colisionCircle = ColisionCircle{
      .cordinate = glm::vec2{pos[0] + (2 * _scale), pos[1] + (1.5f * _scale)},
      .r = 1.5f * _scale};
}

void Player::spaceKeyDown() {
  if (!isKeyPressed) {
    movementInitialPosition = pos[1];
    timer = duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
    isKeyPressed = true;
  }
}

void Player::spaceKeyUp() { isKeyPressed = false; }

void Player::initializeGL(GLuint layerId) {
  terminateGL();

  globalInput.addListener(SDLK_SPACE, SDL_KEYDOWN,
                          std::bind(&Player::spaceKeyDown, this));
  globalInput.addListener(SDLK_SPACE, SDL_KEYUP,
                          std::bind(&Player::spaceKeyUp, this));
  timer = duration_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count();

  renderLayer = layerId;
  colorLoc = abcg::glGetUniformLocation(renderLayer, "color");
  scaleLoc = abcg::glGetUniformLocation(renderLayer, "scale");
  translationLoc = abcg::glGetUniformLocation(renderLayer, "translation");

  // clang-format off
  std::array<glm::vec2, 16> squareShape {
    //BICO 0~3
    glm::vec2{3.0f, 2.0f}, glm::vec2{4.0f, 2.0f},
    glm::vec2{3.0f, 1.0f}, glm::vec2{4.0f, 1.0f},
    //Corpo retangulo vertical 4~7
    glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 3.0f},
    glm::vec2{2.0f, 3.0f}, glm::vec2{2.0f, 0.0f},
    // corpo retangulo horizontal 8~11
    glm::vec2{0.0f, 1.0f}, glm::vec2{0.0f, 2.0f},
    glm::vec2{3.0f, 2.0f}, glm::vec2{3.0f, 1.0f},
    //Triangulos 12~15
    glm::vec2{1.0f, 1.0f}, glm::vec2{1.0f, 2.0f},
    glm::vec2{2.0f, 2.0f}, glm::vec2{2.0f, 1.0f}
  };

  const std::array indices{
                          // Bico
                           0,1,2,
                           2,0,3,
                          // Corpo retangulo vertical
                           4,5,6,
                           4,6,7,
                           // Corpo retangulo horizontal
                           8,9,10,
                           8,10,11,

                           12, 4, 8,
                           13, 9, 5,
                           14, 6, 10,
                           15, 7, 11
                           };
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

void Player::reset() {
  timer = duration_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count();
  movementInitialPosition = _initialPos;
}

void Player::paintGL(int8_t gameState) {
  // if (gameState == 1) {
  updatePosition();
  // }
  printf("%i",gameState);
  abcg::glUseProgram(renderLayer);
  abcg::glBindVertexArray(vao);

  abcg::glUniform1f(scaleLoc, scale);
  abcg::glUniform2fv(translationLoc, 1, &translation.x);

  abcg::glUniform4fv(colorLoc, 1, &color.r);

  abcg::glDrawElements(GL_TRIANGLES, 10 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Player::terminateGL() {
  abcg::glDeleteBuffers(1, &vbo);
  abcg::glDeleteBuffers(1, &ebo);
  abcg::glDeleteVertexArrays(1, &vao);
}
