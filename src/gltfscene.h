#include <glad/gl.h>
#include <stb_image.h>
#include "tiny_gltf.h"

class Scene
{
  public:
      bool load(tinygltf::Model &model, const char* filename);
      void loadAndDrawTriangle();
      void dbgModel(tinygltf::Model &model);
      void loadTextures(tinygltf::Model &model);
};
