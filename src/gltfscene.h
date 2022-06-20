#include <shader.h>
#include <glad/gl.h>
#include <stb_image.h>
#include "tiny_gltf.h"
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Scene
{
  private:
      glm::mat4 view;

  public:
      glm::mat4 projection;
      Shader ourShader;

      Scene(Shader &ourShader);

      bool load(tinygltf::Model &model, const char* filename);
      void loadAndDrawTriangle(glm::mat4 &view);
      void dbgModel(tinygltf::Model &model);
      void loadTextures(tinygltf::Model &model);
      std::pair<GLuint, std::map<int, GLuint>> bindCrude(tinygltf::Model &model); 

      std::string getexepath();
      void bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Node &node);
      void bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Mesh &mesh);

      void drawScene(const std::map<int, GLuint>& vbos, tinygltf::Model &model, glm::mat4 &viewParam);
      void drawNode(tinygltf::Model &model, const tinygltf::Node &node, glm::mat4 matrix, std::map<int, GLuint> vbos);
      void drawMesh(tinygltf::Mesh &mesh, tinygltf::Model &model, glm::mat4 matrix, std::map<int, GLuint> vbos);
      void setView(glm::mat4 &viewParam);
};
