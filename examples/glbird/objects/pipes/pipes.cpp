#include "pipes.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <random>

#include "../../input.hpp"

extern Input globalInput;

Pipes::Pipes() {
  id = -1;
  timer = duration_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count();
}

Pipes::Pipes(int _id, int64_t _timer) {
  id = _id;
  timer = _timer;
}

Pipes::Pipes(int _id, int64_t _timer, glm::vec4 _color) {
  id = _id;
  timer = _timer;
  color = _color;
}

void Pipes::reset() {
  pointWasCount = false;
  pos[0] = 2.0f;
}

void Pipes::initializeGL(GLuint layerId) {
  terminateGL();

  renderLayer = layerId;
  colorLoc = abcg::glGetUniformLocation(renderLayer, "color");
  scaleLoc = abcg::glGetUniformLocation(renderLayer, "scale");
  translationLoc = abcg::glGetUniformLocation(renderLayer, "translation");

  // clang-format off
  std::array<glm::vec2, 16> squareShape {
    //tube 1
    glm::vec2{0.0f, 0.0f}, glm::vec2{tubeWidth, 0.0f},
    glm::vec2{tubeWidth, tubeHeight}, glm::vec2{0.0f, tubeHeight},
    //conector 1
    glm::vec2{-1.0f, tubeHeight}, glm::vec2{-1.0f, 5.0f},
    glm::vec2{3.0f, 5.0f}, glm::vec2{3.0f, tubeHeight},

    //tube 2
    glm::vec2{0.0f, spaceBetweenTubes+tubeHeight+2.0f}, glm::vec2{2.0f, tubeHeight+2.0f+spaceBetweenTubes},
    glm::vec2{2.0f, spaceBetweenTubes+tubeHeight+2.0f+4.0f}, glm::vec2{0.0f, tubeHeight+2.0f+spaceBetweenTubes+4.0f},
    //conector 2
    glm::vec2{-1.0f, spaceBetweenTubes+1.0f+tubeHeight}, glm::vec2{-1.0f, spaceBetweenTubes+1.0f+tubeHeight+1.0f},
    glm::vec2{3.0f, spaceBetweenTubes+1.0f+tubeHeight+1.0f}, glm::vec2{3.0f, spaceBetweenTubes+1.0f+tubeHeight}
  };

  const std::array indices{0,1,2,
                           0,2,3,

                           4,5,6,
                           4,6,7,
                           
                           8,9,10,
                           8,10,11,
                           
                           12,13,14,
                           12,14,15};
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

void Pipes::updatePosition() {
  int64_t t = duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count();
  float movementTime = (t - timer) / 1000.0f;

  pos[0] = INITIAL_POSTION + velocity * movementTime;

  if (pos[0] <= -2.5f) {
    timer = duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
    float random = (rand() % 2 == 0 ? 1 : -1) * ((rand() % 1000) / 1000.0f);
    pointWasCount = false;
    pos[0] = 2.0f;
    pos[1] = (((random)*1.5f + -10.5f) * _Pipes_scale);
  }

  translation = glm::vec2{pos[0], pos[1]};


  glm::vec2 bottomTubePosition{pos[0] - (1 * _Pipes_scale), pos[1]};
  glm::vec2 topTubePosition{
      pos[0] - (1 * _Pipes_scale),
      pos[1] + ((tubeHeight + 1.0f + spaceBetweenTubes) * _Pipes_scale)};
  

  tubeColision = TubesColision{.bottomTubePosition = bottomTubePosition,
                               .topTubePosition = topTubePosition};
  // colisionBox = ColisionBox{.firstCordinate = firstCordinate,
  //                .secondCordinate = secondCordinate};
}

void Pipes::paintGL(int8_t gameState) {
  if (gameState == 1) {
    updatePosition();

    if (!pointWasCount && pos[0] < 0) {
      pointWasCount = true;

      printf("%i: adding point, %f\n", id, pos[0]);
      addPoint();
    }
  }
  abcg::glUseProgram(renderLayer);
  abcg::glBindVertexArray(vao);

  abcg::glUniform1f(scaleLoc, scale);
  abcg::glUniform2fv(translationLoc, 1, &translation.x);

  abcg::glUniform4fv(colorLoc, 1, &color.r);

  abcg::glDrawElements(GL_TRIANGLES, 8 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Pipes::terminateGL() {
  abcg::glDeleteBuffers(1, &vbo);
  abcg::glDeleteBuffers(1, &ebo);
  abcg::glDeleteVertexArrays(1, &vao);
}

void Pipes::registryAddPointFunction(std::function<void()> f) { addPoint = f; }