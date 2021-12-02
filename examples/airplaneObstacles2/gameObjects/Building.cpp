#include "Building.hpp"

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
    const std::size_t h2{std::hash<glm::vec3>()(vertex.normal)};
    const std::size_t h3{std::hash<glm::vec2>()(vertex.texCoord)};
    return h1 ^ h2 ^ h3;
  }
};
}  // namespace std
   // namespace std

void Building::createBuffers() {
  // Delete previous buffers
  abcg::glDeleteBuffers(1, &EBO);
  abcg::glDeleteBuffers(1, &VBO);

  // VBO
  abcg::glGenBuffers(1, &VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
                     vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // EBO
  abcg::glGenBuffers(1, &EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(indices[0]) * indices.size(), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Building::setupVAO() {
  // Release previous VAO
  abcg::glDeleteVertexArrays(1, &VAO);

  // Create VAO
  abcg::glGenVertexArrays(1, &VAO);
  abcg::glBindVertexArray(VAO);

  // Bind EBO and VBO
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Bind vertex attributes
  const GLint positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
  }

  const GLint normalAttribute{abcg::glGetAttribLocation(program, "inNormal")};
  if (normalAttribute >= 0) {
    abcg::glEnableVertexAttribArray(normalAttribute);
    GLsizei offset{sizeof(glm::vec3)};
    abcg::glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void*>(offset));
  }

  const GLint texCoordAttribute{
      abcg::glGetAttribLocation(program, "inTexCoord")};
  if (texCoordAttribute >= 0) {
    abcg::glEnableVertexAttribArray(texCoordAttribute);
    GLsizei offset{sizeof(glm::vec3) + sizeof(glm::vec3)};
    abcg::glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void*>(offset));
  }

  // End of binding
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);
}

void Building::initializeGL(GLuint program, std::string assetsPath) {
  this->program = program;

  loadDiffuseTexture(assetsPath + "airplane_body_diffuse_v1.jpg");
  loadModelFromFile(assetsPath + "11805_airplane_v2_L2.obj",
                    assetsPath + "airplane_body_diffuse_v1.jpg");

  createBuffers();
  setupVAO();
  rederingTypeLocale = abcg::glGetUniformLocation(program, "rederingType");

  // resizeGL(getWindowSettings().width, getWindowSettings().height);
  zeroTime = duration_cast<std::chrono::milliseconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
}

void Building::loadModelFromFile(std::string_view path,
                                 std::string assetsPath) {
  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = assetsPath;  // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data(), readerConfig)) {
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
  const auto& materials{reader.GetMaterials()};

  vertices.clear();
  indices.clear();

  m_hasNormals = false;
  m_hasTexCoords = false;

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

      // Vertex normal
      float nx{};
      float ny{};
      float nz{};
      if (index.normal_index >= 0) {
        m_hasNormals = true;
        const int normalStartIndex{3 * index.normal_index};
        nx = attrib.normals.at(normalStartIndex + 0);
        ny = attrib.normals.at(normalStartIndex + 1);
        nz = attrib.normals.at(normalStartIndex + 2);
      }

      // Vertex texture coordinates
      float tu{};
      float tv{};
      if (index.texcoord_index >= 0) {
        m_hasTexCoords = true;
        const int texCoordsStartIndex{2 * index.texcoord_index};
        tu = attrib.texcoords.at(texCoordsStartIndex + 0);
        tv = attrib.texcoords.at(texCoordsStartIndex + 1);
      }

      Vertex vertex{};
      vertex.position = {vx, vy, vz};
      vertex.normal = {nx, ny, nz};
      vertex.texCoord = {tu, tv};

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

  // Use properties of first material, if available
  if (!materials.empty()) {
    const auto& mat{materials.at(0)};  // First material
    m_Ka = glm::vec4(mat.ambient[0], mat.ambient[1], mat.ambient[2], 1);
    m_Kd = glm::vec4(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1);
    m_Ks = glm::vec4(mat.specular[0], mat.specular[1], mat.specular[2], 1);
    m_shininess = mat.shininess;

    if (!mat.diffuse_texname.empty())
      loadDiffuseTexture(assetsPath + mat.diffuse_texname);
  } else {
    // Default values
    m_Ka = {0.1f, 0.1f, 0.1f, 1.0f};
    m_Kd = {0.7f, 0.7f, 0.7f, 1.0f};
    m_Ks = {1.0f, 1.0f, 1.0f, 1.0f};
    m_shininess = 25.0f;
  }

  if (!m_hasNormals) {
    computeNormals();
  }
}

void Building::loadDiffuseTexture(std::string_view path) {
  // if (!std::filesystem::exists(path)) return;

  abcg::glDeleteTextures(1, &m_diffuseTexture);
  m_diffuseTexture = abcg::opengl::loadTexture(path);
}

void Building::setLightConfig() {
  const GLint lightDirLoc{
      abcg::glGetUniformLocation(program, "lightDirWorldSpace")};
  const GLint shininessLoc{abcg::glGetUniformLocation(program, "shininess")};
  const GLint IaLoc{abcg::glGetUniformLocation(program, "Ia")};
  const GLint IdLoc{abcg::glGetUniformLocation(program, "Id")};
  const GLint IsLoc{abcg::glGetUniformLocation(program, "Is")};
  const GLint KaLoc{abcg::glGetUniformLocation(program, "Ka")};
  const GLint KdLoc{abcg::glGetUniformLocation(program, "Kd")};
  const GLint KsLoc{abcg::glGetUniformLocation(program, "Ks")};

  const auto lightDirRotated{glm::angleAxis(90.0f, glm::vec3(0, 1, 0)) *
                             glm::vec4(-1.0f, -1.0f, -1.0f, 0.0f)};
  abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);
  abcg::glUniform1f(shininessLoc, m_shininess);
  abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);
}

void Building::computeNormals() {
  // Clear previous vertex normals
  for (auto& vertex : vertices) {
    vertex.normal = glm::zero<glm::vec3>();
  }

  // Compute face normals
  for (const auto offset : iter::range<int>(0, indices.size(), 3)) {
    // Get face vertices
    Vertex& a{vertices.at(indices.at(offset + 0))};
    Vertex& b{vertices.at(indices.at(offset + 1))};
    Vertex& c{vertices.at(indices.at(offset + 2))};

    // Compute normal
    const auto edge1{b.position - a.position};
    const auto edge2{c.position - b.position};
    const glm::vec3 normal{glm::cross(edge1, edge2)};

    // Accumulate on vertices
    a.normal += normal;
    b.normal += normal;
    c.normal += normal;
  }

  // Normalize
  for (auto& vertex : vertices) {
    vertex.normal = glm::normalize(vertex.normal);
  }

  m_hasNormals = true;
}

void Building::paintGL() {
  abcg::glUseProgram(program);

  abcg::glActiveTexture(GL_TEXTURE0);
  abcg::glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
  abcg::glUniform1i(rederingTypeLocale, 0);

  // Set minification and magnification parameters
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Set texture wrapping parameters
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Set  light vars
  setLightConfig();
  abcg::glUniform1i(abcg::glGetUniformLocation(program, "diffuseTex"), 0);

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
  // model =
  //     glm::rotate(model, glm::radians(timeElapsed * 0.05f), glm::vec3(0, 0, 1));
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

void Building::move() {
  int64_t actualTime = duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();
  int64_t timeElapsed = actualTime - zeroTime;

  // position
  float r = 3.0f;
  // position.x = r * cosf(glm::radians(timeElapsed * -0.05));
  position.z = timeElapsed * -0.002f;
}

void Building::terminateGL() {
  abcg::glDeleteBuffers(1, &EBO);
  abcg::glDeleteBuffers(1, &VBO);
  abcg::glDeleteVertexArrays(1, &VAO);
}