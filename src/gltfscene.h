#include <glad/glad.h>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

class Scene
{
  public:
      Scene();
      bool load(tinygltf::Model &model, const char* filename);
      void loadAndDrawTriangle();
};
