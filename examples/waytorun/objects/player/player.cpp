#include "player.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "../../input.hpp"

extern Input globalInput;

void Player::up() {
  printf("print\n");
  color = glm::vec4{0.0f, 0.5f, 1.0f, 1.0f};
}

void Player::down() {
  printf("print\n");
  color = glm::vec4{0.0f, 1.0f, 0.0f, 1.0f};
}

void Player::initializeGL(GLuint layerId) {
  terminateGL();

  globalInput.addListener(SDLK_UP, std::bind(&Player::up, this));
  globalInput.addListener(SDLK_DOWN, std::bind(&Player::down, this));

  renderLayer = layerId;
  colorLoc = abcg::glGetUniformLocation(renderLayer, "color");
  rotationLoc = abcg::glGetUniformLocation(renderLayer, "rotation");
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
  abcg::glUseProgram(renderLayer);
  abcg::glBindVertexArray(vao);

  abcg::glUniform1f(scaleLoc, scale);
  abcg::glUniform1f(rotationLoc, rotation);
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