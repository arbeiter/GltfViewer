#include "mesh.hpp"

Mesh::~Mesh()
{
  glDeleteTextures(1, &texture_);
  glDeleteVertexArrays(1, &VAO);
}

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::string path) {
  std::string texture_file = path;
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

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

  texture_ = loadTexture(texture_file);
  glBindVertexArray(0);
}

void Mesh::draw() {
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
