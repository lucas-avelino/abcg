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
  fmt::print("passou\n");

  for (const auto offset : iter::range<int>(0, verticeGroups.size(), 1)) {
    VBO.push_back(0);
    EBO.push_back(0);
    VAO.push_back(0);
  }
  fmt::print("verticeGroups {} \n", verticeGroups.size());
  for (const auto offset : iter::range<int>(0, verticeGroups.size(), 1)) {
    // fmt::print("passou {} \n", offset);
    abcg::glDeleteBuffers(1, &EBO[offset]);
    abcg::glDeleteBuffers(1, &VBO[offset]);

    // VBO
    abcg::glGenBuffers(1, &VBO[offset]);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, VBO.at(offset));
    // fmt::print("passou {} \n", offset);
    abcg::glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(verticeGroups[offset][0]) * verticeGroups[offset].size(),
        verticeGroups[offset].data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
    // fmt::print("passou {} \n", indices[offset].size());
    // EBO

    abcg::glGenBuffers(1, &EBO[offset]);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[offset]);
    abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                       sizeof(indices[offset][0]) * indices[offset].size(),
                       indices[offset].data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // fmt::print("passou {} \n", offset);
  }

  fmt::print("Created Buffers\n");
}

void Building::initGame(glm::vec3 position) {
  this->position = position;
  colisionRect =
      Rectangle{.coord = glm::vec2(position.x - 0.575, position.z - 0.575),
                .size = glm::vec2(1.15f)};
}

void Building::setupVAO(int groupOffset) {
  // for (const auto groupOffset : iter::range<int>(0, verticeGroups.size(), 1))
  // {
  abcg::glDeleteVertexArrays(1, &VAO[groupOffset]);

  // Create VAO
  abcg::glGenVertexArrays(1, &VAO[groupOffset]);

  abcg::glBindVertexArray(VAO[groupOffset]);

  // Bind EBO and VBO
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[groupOffset]);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, VBO[groupOffset]);

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
  // }
}

void Building::initializeGL(GLuint _program, std::string assetsPath) {
  program = _program;
  loadModelFromFile(assetsPath + "building/Building.obj",
                    assetsPath + "building/");

  createBuffers();
  // setupVAO();
  rederingTypeLocale = abcg::glGetUniformLocation(program, "rederingType");

  colisionRect =
      Rectangle{.coord = glm::vec2(position.x - 0.575, position.z - 0.575),
                .size = glm::vec2(1.15f)};
  // 0.575
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

  verticeGroups.clear();
  indices.clear();

  m_hasNormals = false;
  m_hasTexCoords = false;

  // A key:value map with key=Vertex and value=index
  std::vector<std::unordered_map<Vertex, GLuint>> hash{};

  int i = 0;
  fmt::print("{}\n", shapes.size());
  // Loop over shapes
  for (const auto& shape : shapes) {
    fmt::print("Building - shape\n");
    hash.push_back(std::unordered_map<Vertex, GLuint>());
    indices.push_back(std::vector<GLuint>());
    verticeGroups.push_back(std::vector<Vertex>());
    fmt::print("--> {}\n", verticeGroups.size());
    // Loop over indices
    for (const auto offset : iter::range(shape.mesh.indices.size())) {
      // fmt::print("Building - mesh\n");
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
      if (hash.at(i).count(vertex) == 0) {
        // Add this index (size of vertices)
        hash.at(i)[vertex] = verticeGroups.at(i).size();
        // Add this vertex
        // verticeGroups.push_back(std::vector<Vertex>());
        verticeGroups.at(i).push_back(vertex);
      }
      indices.at(i).push_back(hash.at(i)[vertex]);
    }
    fmt::print("Vertex: {}\n", verticeGroups.at(i).size());
    fmt::print("test: {} ", shape.mesh.material_ids[0]);
    fmt::print("material_ids(size): {}   test: {}\n",
               shape.mesh.material_ids.size(), shape.mesh.indices.size());
    i++;
  }

  // Use properties of first material, if available
  if (!materials.empty()) {
    for (const auto groupOffset : iter::range(materials.size())) {
      fmt::print("loading textures {}/{}\n", groupOffset, materials.size());
      const auto& mat{materials.at(groupOffset)};  // First material
      m_Ka.push_back(
          glm::vec4(mat.ambient[0], mat.ambient[1], mat.ambient[2], 1));
      m_Kd.push_back(
          glm::vec4(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1));
      m_Ks.push_back(
          glm::vec4(mat.specular[0], mat.specular[1], mat.specular[2], 1));
      m_shininess.push_back(mat.shininess);

      if (!mat.diffuse_texname.empty()) {
        const auto texture =
            abcg::opengl::loadTexture(assetsPath + mat.diffuse_texname);
        m_diffuseTexture.push_back(texture);
      } else {
        m_diffuseTexture.push_back(0);
      }
    }
  } else {
    // Default values
    // m_Ka = {0.1f, 0.1f, 0.1f, 1.0f};
    // m_Kd = {0.7f, 0.7f, 0.7f, 1.0f};
    // m_Ks = {1.0f, 1.0f, 1.0f, 1.0f};
    // m_shininess = 25.0f;
  }
  fmt::print("textures loaded\n");
  // if (!m_hasNormals) {
    computeNormals();
  // }
  fmt::print("normals computed\n");
}

void Building::loadDiffuseTexture(std::string_view path, int index) {
  // if (!std::filesystem::exists(path)) return;

  abcg::glDeleteTextures(1, &m_diffuseTexture[index]);
  m_diffuseTexture.at(index) = abcg::opengl::loadTexture(path);
}

void Building::setLightConfig(LightProperties light, int textureIndex) {
  const GLint lightDirLoc{
      abcg::glGetUniformLocation(program, "lightDirWorldSpace")};
  const GLint shininessLoc{abcg::glGetUniformLocation(program, "shininess")};
  const GLint IaLoc{abcg::glGetUniformLocation(program, "Ia")};
  const GLint IdLoc{abcg::glGetUniformLocation(program, "Id")};
  const GLint IsLoc{abcg::glGetUniformLocation(program, "Is")};
  const GLint KaLoc{abcg::glGetUniformLocation(program, "Ka")};
  const GLint KdLoc{abcg::glGetUniformLocation(program, "Kd")};
  const GLint KsLoc{abcg::glGetUniformLocation(program, "Ks")};
  const auto lightDirRotated{glm::vec4(100.0f, 100.0f, 100.0f, 10.0f) *
                             light.lighDir};
  abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  abcg::glUniform4fv(IaLoc, 1, &light.Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &light.Id.x);
  abcg::glUniform4fv(IsLoc, 1, &light.Is.x);
  abcg::glUniform1f(shininessLoc, light.shininess);
  abcg::glUniform4fv(KaLoc, 1, &m_Ka[textureIndex].x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd[textureIndex].x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks[textureIndex].x);
}

void Building::computeNormals() {
  for (const auto groupOffset : iter::range(verticeGroups.size() - 1)) {
    for (auto& vertex : verticeGroups[groupOffset]) {
      vertex.normal = glm::zero<glm::vec3>();
    }

    // Compute face normals
    for (const auto offset :
         iter::range<int>(0, verticeGroups[groupOffset].size() - 3, 3)) {
      // fmt::print("{}", verticeGroups[groupOffset][0].);
      // Get face vertices
      Vertex& a = verticeGroups.at(groupOffset).at(offset);
      Vertex& b = verticeGroups.at(groupOffset).at(offset + 1);
      Vertex& c = verticeGroups.at(groupOffset).at(offset + 2);

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
    for (auto& vertex : verticeGroups[groupOffset]) {
      vertex.normal = glm::normalize(vertex.normal);
    }

    m_hasNormals = true;
  }
  // Clear previous vertex normals
}

void Building::paintGL(LightProperties light) {
  abcg::glUseProgram(program);

  glm::mat4 model{1.0f};
  model = glm::translate(model, position + glm::vec3(0, 0, -1.10f)
                         // + glm::vec3(-0.5f, 1.2f, 0)
  );
  model = glm::scale(model, scale);
  abcg::glUniformMatrix4fv(abcg::glGetUniformLocation(program, "modelMatrix"),
                           1, GL_FALSE, &model[0][0]);
  colisionRect =
      Rectangle{.coord = glm::vec2(position.x - 0.575, position.z - 0.575),
                .size = glm::vec2(1.15f)};
  for (const auto groupOffset : iter::range(verticeGroups.size())) {
    setupVAO(groupOffset);
    abcg::glActiveTexture(GL_TEXTURE0);
    if (m_diffuseTexture[groupOffset % m_diffuseTexture.size()] != 0)
      abcg::glBindTexture(
          GL_TEXTURE_2D,
          m_diffuseTexture[groupOffset % m_diffuseTexture.size()]);
    abcg::glUniform1i(rederingTypeLocale, 0);

    // Set minification and magnification parameters
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set texture wrapping parameters
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set  light vars
    setLightConfig(light, groupOffset % m_diffuseTexture.size());
    abcg::glUniform1i(abcg::glGetUniformLocation(program, "diffuseTex"), 0);

    const GLint colorLoc{abcg::glGetUniformLocation(program, "color")};

    abcg::glBindVertexArray(VAO[groupOffset]);

    abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
    abcg::glDrawElements(GL_TRIANGLES, indices[groupOffset].size(),
                         GL_UNSIGNED_INT, nullptr);
  }
}

void Building::terminateGL() {
  for (const auto offset : iter::range(verticeGroups.size())) {
    abcg::glDeleteBuffers(1, &EBO[offset]);
    abcg::glDeleteBuffers(1, &VBO[offset]);
    abcg::glDeleteVertexArrays(1, &VAO[offset]);
  }
}