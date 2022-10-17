#ifndef MESH_HPP
#define MESH_HPP
#include <glad/gl.h>
#include <vector>
#include "texture.hpp"
#include <string_view>
#include <glm.hpp>

struct Vertex {
  glm::vec3 Position{0.0f};
  glm::vec3 Normal{0.0f};
  glm::vec2 TexCoords{0.0f};
};

class Mesh {
  public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::string path);
    Mesh();
    ~Mesh();
    void draw();
    Mesh createCubeMesh(const glm::vec3& dimensions);

  private:
    const char* texture_file;
    GLuint VAO, VBO, EBO;
    void setupMesh();
    unsigned int texture_ = 0;
    bool textureEnabled;
};

#endif
