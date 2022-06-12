#include "gltfscene.h"
#include <iostream>
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::string Scene::getexepath()
{
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  return std::string( result, (count > 0) ? count : 0 );
}


bool Scene::load(tinygltf::Model &model, const char *filename) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;
  bool ret = false;
  ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
  if (!err.empty()) std::cout << "ERR : " << err << std::endl;
  return ret;
}

void Scene::dbgModel(tinygltf::Model &model) {
  for (auto &mesh : model.meshes) {
    std::cout << "mesh : " << mesh.name << std::endl;
    for (auto &primitive : mesh.primitives) {
      const tinygltf::Accessor &indexAccessor =
          model.accessors[primitive.indices];

      std::cout << "indexaccessor: count " << indexAccessor.count << ", type "
                << indexAccessor.componentType << std::endl;

      /*
      No support for materials yet
      tinygltf::Material &mat = model.materials[primitive.material];
      for (auto &mats : mat.values) {
        std::cout << "mat : " << mats.first.c_str() << std::endl;
      }

      for (auto &image : model.images) {
        std::cout << "image name : " << image.uri << std::endl;
        std::cout << "  size : " << image.image.size() << std::endl;
        std::cout << "  w/h : " << image.width << "/" << image.height
                  << std::endl;
      }
      */

      std::cout << "indices : " << primitive.indices << std::endl;
      std::cout << "mode     : "
                << "(" << primitive.mode << ")" << std::endl;

      for (auto &attrib : primitive.attributes) {
        std::cout << "attribute : " << attrib.first.c_str() << std::endl;
      }
    }
  }
}

void Scene::loadAndDrawTriangle(Shader &ourShader, glm::mat4 &view) {
  tinygltf::Model model;
  std::string filename = "resources/models/test2/TwoTriangles.gltf";
  if (!load(model, filename.c_str())) {
    std::cout << getexepath() << std::endl;
    std::cout << "File could not be found " << std::endl;
    return;
  }

  std::cout << "File found" << std::endl;
  dbgModel(model);
  std::pair<GLuint, std::map<int, GLuint>> vaoAndEbos = bindCrude(model);
  drawScene(vaoAndEbos.second, model, ourShader, view);
}

void Scene::drawNode(tinygltf::Node &node, glm::mat4 matrix) {
}


void Scene::drawMesh(tinygltf::Mesh &mesh, glm::mat4 matrix) {
}

void Scene::drawScene(const std::map<int, GLuint>& vbos, tinygltf::Model &model, Shader &ourShader, glm::mat4 &view) {
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
  glm::mat4 model_mat(1.0f);
  glm::vec3 model_pos = glm::vec3(-3, 0, -3);

  for (const tinygltf::Scene& scene : model.scenes) {
    for(size_t i = 0; i < scene.nodes.size(); i++) {
      const tinygltf::Node &node = model.nodes[scene.nodes[i]];
      tinygltf::Mesh &mesh = model.meshes[node.mesh];
      if(node.translation.size() == 3) {
        model_mat = glm::translate(model_mat, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
      }
      model_mat = glm::rotate(model_mat, glm::radians(0.8f), glm::vec3(0, 1, 0));  // rotate model on y axis

      ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
      ourShader.setMat4("model", model_mat);
      ourShader.setMat4("view", view);
      ourShader.setMat4("projection", projection);

      for (size_t i = 0; i < mesh.primitives.size(); ++i) {
          tinygltf::Primitive primitive = mesh.primitives[i];
          if(primitive.indices > -1) {
            tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));
            glDrawElements(GL_TRIANGLES, 3, indexAccessor.componentType, BUFFER_OFFSET(indexAccessor.byteOffset));
          }
      }
    }
  }
}

std::pair<GLuint, std::map<int, GLuint>> Scene::bindCrude(tinygltf::Model &model) {
  GLuint vao;
  std::map<int, GLuint> vbos;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  for(size_t i = 0; i < scene.nodes.size(); ++i) {
    tinygltf::Node &node = model.nodes[scene.nodes[i]];
    bindModelNodes(vbos, model, node);
  }
  return {vao, vbos};
}

void Scene::bindModelNodes(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Node &node) {
  // for node in nodes
  // if termination condition, bindMesh
  if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
    bindMesh(vbos, model, model.meshes[node.mesh]);
  }

  for (size_t i = 0; i < node.children.size(); i++) {
    assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
    bindModelNodes(vbos, model, model.nodes[node.children[i]]);
  }
}

void Scene::bindMesh(std::map<int, GLuint>& vbos,
                      tinygltf::Model &model, tinygltf::Mesh &mesh) {

  for(size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if(bufferView.target == 0) {
      std::cout << "WARN" << std::endl;
      continue;
    }

    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
    GLuint vbo;
    glGenBuffers(1, &vbo);
    vbos[i] = vbo;
    glBindBuffer(bufferView.target, vbo);
    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
  }

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
      tinygltf::Primitive primitive = mesh.primitives[i];
      tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
      for (auto &attrib : primitive.attributes) {
          tinygltf::Accessor accessor = model.accessors[attrib.second];
          int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
          glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);
          int size = 1;
          if (accessor.type != TINYGLTF_TYPE_SCALAR) {
            size = accessor.type;
          }
          int vaa = -1;
          if (attrib.first.compare("POSITION") == 0) vaa = 0;
          if (attrib.first.compare("NORMAL") == 0) vaa = 1;
          if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
          if(vaa > -1) {
              glEnableVertexAttribArray(vaa);
              glVertexAttribPointer(vaa, size, accessor.componentType,
                                    accessor.normalized ? GL_TRUE : GL_FALSE,
                                    byteStride, BUFFER_OFFSET(accessor.byteOffset));
          }
      }
  }

  if (model.textures.size() > 0) {
    loadTextures(model);
  }
}

void Scene::loadTextures(tinygltf::Model &model) {
    const std::vector<tinygltf::Texture>& textures = model.textures;
    for(const tinygltf::Texture& texture : textures) {
      if (texture.source > -1) {
        GLuint texid;
        glGenTextures(1, &texid);

        tinygltf::Image &image = model.images[texture.source];
        // TODO: Text mipmapping
        /*
          glGenerateMipmap(GL_TEXTURE_2D);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
         */

        glBindTexture(GL_TEXTURE_2D, texid);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // TODO: Fix this
        GLenum format = GL_RGBA;
        if (image.component == 1) {
          format = GL_RED;
        } else if (image.component == 2) {
          format = GL_RG;
        } else if (image.component == 3) {
          format = GL_RGB;
        }

        GLenum type = GL_UNSIGNED_BYTE;
        if (image.bits == 16) {
          type = GL_UNSIGNED_SHORT;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                     format, type, &image.image.at(0));
      }
    }
}
