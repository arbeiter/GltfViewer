#include <glad/gl.h>
#include <stb_image.h>
#include "tiny_gltf.h"
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Scene
{
  public:
      bool load(tinygltf::Model &model, const char* filename);
      void loadAndDrawTriangle();
      void dbgModel(tinygltf::Model &model);
      void loadTextures(tinygltf::Model &model);
      std::pair<GLuint, std::map<int, GLuint>> bindCrude(tinygltf::Model &model); 

      std::string getexepath();
      void bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Node &node);
      void bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Mesh &mesh);

      void drawScene(const std::map<int, GLuint>& vbos, tinygltf::Model &model);
      void drawNode(tinygltf::Node &node, glm::mat4 matrix);
      void drawMesh(tinygltf::Mesh &mesh, glm::mat4 matrix);
};
