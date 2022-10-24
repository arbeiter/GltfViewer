#ifndef GLTF_SCENE_HPP
#define GLTF_SCENE_HPP
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
#include <gtx/string_cast.hpp>

struct TextureValues {
  bool isMetallicTexture;
  bool isBaseTexture;
};

class Scene
{
  private:
      glm::mat4 view;
      int width;
      int height;
      std::string filename;
      const float SCR_WIDTH = 3440.0f;
      const float SCR_HEIGHT = 1440.0f;

      tinygltf::Model internalModel;
      std::pair<GLuint, std::map<int, GLuint>> vaoAndEbos;
      std::vector<GLuint> allTextures;
      bool loadGltf(tinygltf::Model &model, const char* filename);

      typedef struct {
        GLuint vb;
      } GLBufferState;
      std::map<int, GLBufferState> gBufferState;
      typedef struct {
        std::map<std::string, GLint> attribs;
        std::map<std::string, GLint> uniforms;
      } GLProgramState;
      GLProgramState gGLProgramState;

  public:
      glm::mat4 projection;
      Shader ourShader;

      Scene(Shader &ourShader, std::string resourcePath);
      void loadModel(glm::mat4 &view, int modelNumber, Shader &shader);
      void dbgModel(tinygltf::Model &model);
      void loadTextures(tinygltf::Model &model);
      std::pair<GLuint, std::map<int, GLuint>> bindCrude(tinygltf::Model &model, Shader &shader);

      std::string getexepath();
      void bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Node &node);
      void bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Mesh &mesh);

      void setShader(Shader &shader, glm::vec3 &viewPos);
      void setMaterials(tinygltf::Material &material, Shader& ourShader);
      void drawScene(glm::mat4 &viewParam);
      void drawNode(tinygltf::Model &model, const tinygltf::Node &node, glm::mat4 matrix, std::map<int, GLuint> vbos);
      void drawMesh(tinygltf::Mesh &mesh, tinygltf::Model &model, glm::mat4 matrix, std::map<int, GLuint> vbos);
      void setView(glm::mat4 &viewParam);
      void setWidthAndHeight(int w, int h);
};
#endif
