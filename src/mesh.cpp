#include "mesh.hpp"

Mesh::~Mesh()
{
  if(textureEnabled)
    glDeleteTextures(1, &texture_);
  glDeleteVertexArrays(1, &VAO);
}

Mesh::Mesh(const std::string& path)
{
  textureEnabled = false;
  vertices = {};
  indices = {};
  texture_file = path;
}

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, const std::string& path) {
  textureEnabled = true;
  texture_file = path;
  vertices = vertices;
  indices = indices;
}

void Mesh::setupMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

  //vertices
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
  // normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

  //textures
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

  if(textureEnabled)
    texture_ = loadTexture(texture_file.c_str());
}

// TODO: Move to base class
void Mesh::draw(Shader& simpleShader, glm::mat4 &view, glm::vec3 &pos) {
  simpleShader.use();
  glm::mat4 projection = glm::perspective(glm::radians(30.0f), (float)(3440/ 1440), 0.1f, 1000.0f);

  glBindVertexArray(VAO);
  simpleShader.setMat4("view", view);
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, pos);
  simpleShader.setMat4("model", model);
  simpleShader.setMat4("projection", projection);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void Mesh::draw() {
  if(textureEnabled)
    glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(VAO);
  if(textureEnabled)
    glBindTexture(GL_TEXTURE_2D, texture_);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void Mesh::initCubeWithDimensions(const glm::vec3& dimensions) {
  textureEnabled = true;
  float w = dimensions.x;
  float h = dimensions.y;
  float d = dimensions.z;

  // clang-format off
  vertices = {
      {{w, h, d}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
      {{0, h, d}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
      {{0, 0, d}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
      {{w, 0, d}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

      {{0, h, d}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
      {{0, h, 0}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
      {{0, 0, 0}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
      {{0, 0, d}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},

      {{0, h, 0}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
      {{w, h, 0}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
      {{w, 0, 0}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
      {{0, 0, 0}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},

      {{w, h, 0}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
      {{w, h, d}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
      {{w, 0, d}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
      {{w, 0, 0}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},

      {{w, h, 0}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
      {{0, h, 0}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
      {{0, h, d}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
      {{w, h, d}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

      {{0, 0, 0}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
      {{w, 0, 0}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
      {{w, 0, d}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
      {{0, 0, d}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
  };
  // clang-format on

  int currIndex = 0;
  for (int i = 0; i < 6; i++) {
      indices.push_back(currIndex);
      indices.push_back(currIndex + 1);
      indices.push_back(currIndex + 2);
      indices.push_back(currIndex + 2);
      indices.push_back(currIndex + 3);
      indices.push_back(currIndex);
      currIndex += 4;
  }
  setupMesh();
}
