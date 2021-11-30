#include "Airplane.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

#include "../utils/input.hpp"
#include "abcg.hpp"

// Explicit specialization of std::hash for Vertex
namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const noexcept {
    const std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};
}  // namespace std

void Airplane::initializeGL(GLuint program, std::string assetsPath) {
  // abcg::glClearColor(0, 0, 0, 1);

  // // Enable depth buffering
  // abcg::glEnable(GL_DEPTH_TEST);

  // Create program

  this->program = program;

  // Load model
  loadModelFromFile(assetsPath + "airplane.obj");

  // Generate VBO
  abcg::glGenBuffers(1, &VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
                     vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(indices[0]) * indices.size(), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, VBO);
  const GLint positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  // resizeGL(getWindowSettings().width, getWindowSettings().height);
  zeroTime = duration_cast<std::chrono::milliseconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
}

void Airplane::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  vertices.clear();
  indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over indices
    for (const auto offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      const tinyobj::index_t index{shape.mesh.indices.at(offset)};

      // Vertex position
      const int startIndex{3 * index.vertex_index};
      const float vx{attrib.vertices.at(startIndex + 0)};
      const float vy{attrib.vertices.at(startIndex + 1)};
      const float vz{attrib.vertices.at(startIndex + 2)};

      Vertex vertex{};
      vertex.position = {vx, vy, vz};

      // If hash doesn't contain this vertex
      if (hash.count(vertex) == 0) {
        // Add this index (size of vertices)
        hash[vertex] = vertices.size();
        // Add this vertex
        vertices.push_back(vertex);
      }

      indices.push_back(hash[vertex]);
    }
  }
}

void Airplane::paintGL() {

  abcg::glUseProgram(program);


  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(program, "modelMatrix")};
  const GLint colorLoc{abcg::glGetUniformLocation(program, "color")};

  abcg::glBindVertexArray(VAO);
  int64_t actualTime = duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();

  int64_t timeElapsed = actualTime - zeroTime;
  glm::mat4 model{1.0f};
  move();

  model = glm::translate(model, position);
  model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
  model =
      glm::rotate(model, glm::radians(timeElapsed * 0.05f), glm::vec3(0, 0, 1));
  model = glm::scale(model, glm::vec3(0.001f));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

  max = glm::vec3(std::numeric_limits<float>::lowest());
  min = glm::vec3(std::numeric_limits<float>::max());
  for (const auto& vertex : vertices) {
    max.x = std::max(max.x, vertex.position.x);
    max.y = std::max(max.y, vertex.position.y);
    max.z = std::max(max.z, vertex.position.z);
    min.x = std::min(min.x, vertex.position.x);
    min.y = std::min(min.y, vertex.position.y);
    min.z = std::min(min.z, vertex.position.z);
  }
  airplaneSize = max - min;
}

void Airplane::move() {
  int64_t actualTime = duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();
  int64_t timeElapsed = actualTime - zeroTime;

  // position
  float r = 3.0f;
  position.x = r * cosf(glm::radians(timeElapsed * -0.05));
  position.z = r * sinf(glm::radians(timeElapsed * -0.05));

}

void Airplane::terminateGL() {
  abcg::glDeleteBuffers(1, &EBO);
  abcg::glDeleteBuffers(1, &VBO);
  abcg::glDeleteVertexArrays(1, &VAO);
}