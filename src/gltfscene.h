#include <glad/gl.h>
#include <stb_image.h>
#include "tiny_gltf.h"

class Scene
{
  public:
      Scene();
      bool load(tinygltf::Model &model, const char* filename);
      void loadAndDrawTriangle();
};
