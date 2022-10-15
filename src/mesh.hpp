#ifndef MESH_HPP
#define MESH_HPP
#include <glad/gl.h>
#include <vector>
#include "texture.hpp"
#include <string_view>
#include <glm.hpp>

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

class Mesh {
  public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::string path);
    ~Mesh();
    void draw();

  private:
    const char* texture_file;
    GLuint VAO, VBO, EBO;
    void setupMesh();
    unsigned int texture_ = 0;
};
#endif
