#include <glad/gl.h>
#include <stb_image.h>
#include "tiny_gltf.h"
#include <string.h>
#include <limits.h>
#include <unistd.h>

class Scene
{
  public:
      bool load(tinygltf::Model &model, const char* filename);
      void loadAndDrawTriangle();
      void dbgModel(tinygltf::Model &model);
      void loadTextures(tinygltf::Model &model);
      void drawCrude(const std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Mesh &mesh);
      std::pair<GLuint, std::map<int, GLuint>> bindCrude(tinygltf::Model &model); 
      std::string getexepath();
      void bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Node &node);
      void bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Mesh &mesh);
};
