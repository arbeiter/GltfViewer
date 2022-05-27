#include "gltfscene.h"
#include <iostream>
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

bool Scene::load(tinygltf::Model &model, const char *filename) {

  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;
  bool ret = false;
  ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
  if (!err.empty()) std::cout << "ERR: " << std::endl;
  return ret;
}

void Scene::loadAndDrawTriangle() {
  tinygltf::Model model;
  std::string filename = "resources/triangle.gltf";
  if (!load(model, filename.c_str())) return;

  GLuint vao;
  std::map<int, GLuint> vbos;

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  tinygltf::Node &node = model.nodes[scene.nodes[0]];
  tinygltf::Mesh &mesh = model.meshes[node.mesh];

  for(size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if(bufferView.target == 0) {
      std::cout << "WARN" << std::endl;
      continue;
    }

    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
    GLuint vbo;
    vbos[i] = vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(bufferView.target, vbo);
    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
  }

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
      tinygltf::Primitive primitive = mesh.primitives[i];
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
          if(vaa > -1) {
              glEnableVertexAttribArray(vaa);
              glVertexAttribPointer(vaa, size, accessor.componentType,
                                    accessor.normalized ? GL_TRUE : GL_FALSE,
                                    byteStride, BUFFER_OFFSET(accessor.byteOffset));
          }
      }
  }

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
      tinygltf::Primitive primitive = mesh.primitives[i];
      tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));
      glDrawElements(primitive.mode, indexAccessor.count,
                     indexAccessor.componentType,
                     BUFFER_OFFSET(indexAccessor.byteOffset));
  }
  glBindVertexArray(0);
};
