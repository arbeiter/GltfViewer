#ifndef MESH_HPP
#define MESH_HPP
#include "shader.h"
#include "texture.hpp"
#include <glad/gl.h>
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

struct Vertex {
  glm::vec3 Position{0.0f};
  glm::vec2 TexCoords{0.0f};
  glm::vec3 Normal{0.0f};
};

class Mesh {
  public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, const std::string& path);
    Mesh(const std::string& path);
    ~Mesh();
    void draw();
    void draw(Shader& shader, glm::mat4& view);
    std::string texture_file;
    GLuint VAO, VBO, EBO;
    void setupMesh();
    void initCubeWithDimensions(const glm::vec3& dimensions);
    unsigned int texture_ = 0;
    bool textureEnabled;
};

#endif
