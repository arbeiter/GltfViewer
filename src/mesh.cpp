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

Mesh::Mesh(std::string type) {
  // multiplex on type -> shape
  createCubeMesh(glm::vec3({10, 10, 10}));
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

Mesh Shape::createCubeMesh(const glm::vec3& dimensions) {
    Mesh mesh;

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
        mesh.indices.push_back(currIndex);
        mesh.indices.push_back(currIndex + 1);
        mesh.indices.push_back(currIndex + 2);
        mesh.indices.push_back(currIndex + 2);
        mesh.indices.push_back(currIndex + 3);
        mesh.indices.push_back(currIndex);
        currIndex += 4;
    }
    return mesh;
}
