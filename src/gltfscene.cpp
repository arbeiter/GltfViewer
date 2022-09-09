#include "gltfscene.h"
#include <iostream>
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
int count = 0;

typedef struct {
  GLuint vb;
} GLBufferState;
std::map<int, GLBufferState> gBufferState;
typedef struct {
  std::map<std::string, GLint> attribs;
  std::map<std::string, GLint> uniforms;
} GLProgramState;
GLProgramState gGLProgramState;

Scene::Scene(Shader &shader): ourShader(shader) {
  width = 800;
  height = 600;
  float four_d[16];
  projection = glm::make_mat4(four_d);
  view = glm::make_mat4(four_d);
}

std::string Scene::getexepath()
{
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  return std::string( result, (count > 0) ? count : 0 );
}

bool Scene::loadGltf(tinygltf::Model &model, const char *filename) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;
  bool ret = false;
  ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
  if (!err.empty()) std::cout << "ERR : " << err << std::endl;
  dbgModel(model);
  return ret;
}

void Scene::dbgModel(tinygltf::Model &model) {
  for (auto &mesh : model.meshes) {
    std::cout << "mesh : " << mesh.name << std::endl;
    for (auto &primitive : mesh.primitives) {
      std::cout << "indices : " << primitive.indices << std::endl;
      const tinygltf::Accessor &indexAccessor =
          model.accessors[primitive.indices];

      std::cout << "indexaccessor: count " << indexAccessor.count << ", type "
                << indexAccessor.componentType << std::endl;

      /*
      tinygltf::Material &mat = model.materials[primitive.material];
      for (auto &mats : mat.values) {
        std::cout << "mat : " << mats.first.c_str() << std::endl; }

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

void Scene::setWidthAndHeight(int w, int h) {
  width = w;
  height = h;
}

void Scene::loadModel(glm::mat4 &view, int elem) {
  tinygltf::Model model;
  elem = 16;
  std::string modelNumber = std::to_string(elem);
  std::string folderName = "";

  std::string altFileName2 = "resources/deccer-cubes/SM_Deccer_Cubes_Textured.gltf";
  std::string altFileName1 = "resources/models/simplified_mesh.gltf";
  std::string altFileName = "resources/models/wheel_with_two_objects.gltf";
  std::string rock = "resources/models/rock/rock.gltf";
  std::string filename = "resources/models/test" + modelNumber + "/" + modelNumber + ".gltf";
  //std::cout << "Attempting to load " << filename << " " << std::endl;
  if (!loadGltf(model, rock.c_str())) {
    std::cout << getexepath() << std::endl;
    std::cout << "File could not be found " << filename << " " << std::endl;
    return;
  }

  vaoAndEbos = bindCrude(model);
  internalModel = model;
  ourShader.use();
  ourShader.setInt("samp_tex", 0);
  ourShader.setInt("metallicTex", 1);
  ourShader.setInt("normalTex", 2);
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

void Scene::bindMesh(std::map<int, GLuint>& vbos, tinygltf::Model &model, tinygltf::Mesh &mesh) {
  for(size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if(bufferView.target == 0) {
      std::cout << "WARN" << std::endl;
      continue;
    }

    int sparse_accessor = -1;
    for (size_t a_i = 0; a_i < model.accessors.size(); ++a_i) {
        const auto &accessor = model.accessors[a_i];
        if (accessor.bufferView == i) {
          // std::cout << i << " is used by accessor " << a_i << std::endl;
          if (accessor.sparse.isSparse) {
            std::cout
                << "WARN: this bufferView has at least one sparse accessor to "
                   "it. We are NOT going to load the data as patched by this "
                   "sparse accessor"
                << std::endl;
            sparse_accessor = a_i;
            exit(0);
          }
        }
    }

    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
    GLBufferState state;
    glGenBuffers(1, &state.vb);
    glBindBuffer(bufferView.target, state.vb);
    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset,
                 GL_STATIC_DRAW);
    glBindBuffer(bufferView.target, 0);
    gBufferState[i] = state;
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

        glBindTexture(GL_TEXTURE_2D, texid);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
        glGenerateMipmap(GL_TEXTURE_2D);
        allTextures.push_back(texid);
        glBindTexture(GL_TEXTURE_2D, 0);
      }
    }
}

void Scene::drawNode(tinygltf::Model &model, const tinygltf::Node &node, glm::mat4 matrix, std::map<int, GLuint> vbos) {

      //std::cout << "Drawing node " << node.name << std::endl;
      glm::mat4 t(1.0f);
      glm::mat4 r(1.0f);
      glm::mat4 s(1.0f);
      glm::mat4 gltf_mat(1.0f);
      if(node.matrix.size() == 16) {
        /*
          gltf_mat = glm::mat4(node.matrix[0], node.matrix[1],
                    node.matrix[2], node.matrix[3], node.matrix[4],
                    node.matrix[5], node.matrix[6], node.matrix[7],
                    node.matrix[8], node.matrix[9], node.matrix[10],
                    node.matrix[11], node.matrix[12], node.matrix[13],
                    node.matrix[14], node.matrix[15]);
        for (uint j = 0; j < 16; j++) {
            gltf_mat[j/4][j%4] = static_cast<float>(node.matrix[j]);
        }
        */
        gltf_mat = glm::make_mat4x4(node.matrix.data());
        gltf_mat = matrix * gltf_mat;
      } else {
        if(node.translation.size() == 3) {
          matrix = glm::translate(matrix, glm::vec3(glm::make_vec3(node.translation.data())));
        }
        if (node.rotation.size() == 4) {
          glm::quat q = glm::make_quat(node.rotation.data());
          matrix *= glm::mat4_cast(q);
        }
        if(node.scale.size() == 3) {
          //std::cout << "Scale value" << std::endl;
          matrix = glm::scale(matrix, glm::vec3(glm::make_vec3(node.scale.data())));
        }
        gltf_mat = matrix;
      }

      for(size_t i = 0; i < node.children.size(); ++i)
      {
        const tinygltf::Node child = model.nodes[node.children[i]];
        drawNode(model, child, gltf_mat, vbos);
      }

      if(node.mesh > -1) {
        tinygltf::Mesh &mesh = model.meshes[node.mesh];
        //std::cout << "Drawing Mesh " << node.name << " " << mesh.name << " " << std::endl;
        drawMesh(mesh, model, gltf_mat, vbos);
      }
}

void Scene::drawMesh(tinygltf::Mesh &mesh, tinygltf::Model &model, glm::mat4 matrix, std::map<int, GLuint> vbos) {
  //std::cout << "count " << count << std::endl;
  count += 1;
  projection = glm::perspective(glm::radians(30.0f), (float)(width / height), 0.04991f, 10000000.0f);

  ourShader.setMat4("model", matrix);
  ourShader.setMat4("view", view);
  ourShader.setMat4("projection", projection);

  glm::mat3 mvi = glm::mat3(glm::transpose(glm::inverse(matrix * view)));
  ourShader.setMat3("MVI", mvi);

  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
      tinygltf::Primitive primitive = mesh.primitives[i];
      for (auto &attrib : primitive.attributes) {
          tinygltf::Accessor accessor = model.accessors[attrib.second];
          glBindBuffer(GL_ARRAY_BUFFER, gBufferState[accessor.bufferView].vb);

          int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
          assert(byteStride != -1);

          int size = 1;
          if (accessor.type != TINYGLTF_TYPE_SCALAR) {
            size = accessor.type;
          } else if (accessor.type == TINYGLTF_TYPE_VEC2) {
            size = 2;
          } else if (accessor.type == TINYGLTF_TYPE_VEC3) {
            size = 3;
          } else if (accessor.type == TINYGLTF_TYPE_VEC4) {
            size = 4;
          } else {
            assert(0);
          }

          int vaa = -1;
          if (attrib.first.compare("POSITION") == 0) vaa = 0;
          if (attrib.first.compare("NORMAL") == 0) vaa = 1;
          if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
          if (attrib.first.compare("COLOR_0") == 0) vaa = 3;
          if (attrib.first.compare("TANGENT") == 0) vaa = 4;
          if(vaa > -1) {
              glEnableVertexAttribArray(vaa);
              glVertexAttribPointer(vaa, size, accessor.componentType,
                                    accessor.normalized ? GL_TRUE : GL_FALSE,
                                    byteStride, BUFFER_OFFSET(accessor.byteOffset));
              gGLProgramState.attribs[attrib.first] = vaa;
          }
      }


      if(primitive.indices > -1) {
        //std::cout << "Mesh: Primitive not none" << std::endl;

        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
        int buffer_type = model.bufferViews[indexAccessor.bufferView].target;
        const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
        const tinygltf::Buffer &indexBuffer = model.buffers[indexBufferView.buffer];

        if(buffer_type == GL_ARRAY_BUFFER) {
          exit(0);
        } else {
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gBufferState[indexAccessor.bufferView].vb);
          int mode = -1;
          if (primitive.mode == TINYGLTF_MODE_TRIANGLES) {
            mode = GL_TRIANGLES;
          } else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
            mode = GL_TRIANGLE_STRIP;
          } else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_FAN) {
            mode = GL_TRIANGLE_FAN;
          } else if (primitive.mode == TINYGLTF_MODE_POINTS) {
            mode = GL_POINTS;
          } else if (primitive.mode == TINYGLTF_MODE_LINE) {
            mode = GL_LINES;
          } else if (primitive.mode == TINYGLTF_MODE_LINE_LOOP) {
            mode = GL_LINE_LOOP;
          } else {
            assert(0);
          }

          if(primitive.material >= 0) {
            tinygltf::Material &mat = model.materials[primitive.material];
            setMaterials(mat, ourShader);
          }
          //std::cout << "Mesh: Index count " << indexAccessor.count << " " << indexAccessor.componentType << " " << std::endl;
          glDrawElements(mode, indexAccessor.count, indexAccessor.componentType, BUFFER_OFFSET(indexAccessor.byteOffset));

          // v3: glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType, reinterpret_cast<void*>(0 + indexAccessor.byteOffset));
          //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
      }
  }
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(4);
  //std::cout << "End render" << std::endl;

  /*
  {
    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
        tinygltf::Primitive primitive = mesh.primitives[i];
        int vaa = -1;
        for (auto &attrib : primitive.attributes) {
          if (attrib.first.compare("POSITION") == 0) vaa = 0;
          if (attrib.first.compare("NORMAL") == 0) vaa = 1;
          if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
          if (attrib.first.compare("COLOR_0") == 0) vaa = 3;
          if (attrib.first.compare("TANGENT") == 0) vaa = 4;
          if(vaa >= 0) {
            glDisableVertexAttribArray(gGLProgramState.attribs[attrib.first]);
          }
        }
      }
  }
  */

}

void Scene::setMaterials(tinygltf::Material &material, Shader& ourShader) {

  bool isBaseColorAbsent = true;
  bool isMetallicFactorAbsent = true;
  bool isRoughFactorAbsent = true;
  bool isbaseColorTexturePresent = false;
  bool isMetallicTexturePresent = false;
  bool isNormalTexturePresent = false;
  float metallicFactorSet = 0.f;
  float roughFactorSet = 0.f;

  for (auto &value : material.values) {
    if (value.first == "baseColorTexture")
    {
        if (value.second.TextureIndex() > -1) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, allTextures[value.second.TextureIndex()]);
        }
        isbaseColorTexturePresent = true;
    }
    else if (value.first == "metallicRoughnessTexture")
    {
        if (value.second.TextureIndex() > -1) {
          glActiveTexture(GL_TEXTURE0 + 1);
          glBindTexture(GL_TEXTURE_2D, allTextures[value.second.TextureIndex()]);
        }
        isMetallicTexturePresent = true;
    }
    else if (value.first == "baseColorFactor")
    {
        std::vector<float> vec = {
          (float)value.second.number_array[0],
          (float)value.second.number_array[1],
          (float)value.second.number_array[2],
          (float)value.second.number_array[3]
        };
        glm::vec3 test = glm::vec3(vec[0], vec[1], vec[2]);
        ourShader.setVec3("base_color_provided", test);
        isBaseColorAbsent = false;
    }
    else if (value.first == "metallicFactor")
    {
      float number = value.second.number_value;
      ourShader.setFloat("metallicFactor", number);
      isMetallicTexturePresent = true;
    }
    else if (value.first == "roughnessFactor")
    {
      float number = value.second.number_value;
      ourShader.setFloat("roughFactor", number);
      //roughFactorSet = number;
      isRoughFactorAbsent = false;
    }
  }

  // TODO: Set to uint
  glm::vec3 whiteFactor = glm::vec3(1.0f, 1.0f, 1.0f);
  if(isRoughFactorAbsent) {
    ourShader.setFloat("roughFactor", 0.0f);
    roughFactorSet = 1.f;
  }
  if(isMetallicFactorAbsent) {
    metallicFactorSet = 0.0f;
    ourShader.setFloat("metallicFactor", 0.0f);
  }
  if(isBaseColorAbsent) {
    ourShader.setVec3("base_color_provided", whiteFactor);
  }

  for (auto &value : material.additionalValues)
  {
    if (value.first == "normalTexture")
    {
      isNormalTexturePresent = true;
      glActiveTexture(GL_TEXTURE0 + 2);
      glBindTexture(GL_TEXTURE_2D, allTextures[value.second.TextureIndex()]);
    }
  }
  ourShader.setBool("isbaseColorTexturePresent", isbaseColorTexturePresent);
  ourShader.setBool("isRoughFactorAbsent", isRoughFactorAbsent);
  ourShader.setBool("isMetallicTexturePresent", isMetallicTexturePresent);
  ourShader.setBool("isNormalTexturePresent", isNormalTexturePresent);
  //std::cout << "XIDC " << isBaseColorAbsent << " " << isRoughFactorAbsent << " " << isMetallicTexturePresent << " " << isNormalTexturePresent << std::endl;
}

void Scene::setView(glm::mat4 &viewParam) {
  //std::cout << glm::to_string(viewParam) << std::endl;
  view = viewParam;
}

void Scene::drawScene(glm::mat4 &viewParam) {
  ourShader.use();
  glm::vec3 v_position = glm::vec3(projection[3][0], projection[3][1], projection[3][2]);
  ourShader.setVec3("light_pos", v_position);

  setView(viewParam);
  glm::mat4 model_mat(1.0f);
  count = 0;
  for (const tinygltf::Scene& scene : internalModel.scenes) {
    for(size_t i = 0; i < scene.nodes.size(); i++) {
      const tinygltf::Node &node = internalModel.nodes[scene.nodes[i]];
      drawNode(internalModel, node, model_mat, vaoAndEbos.second);
    }
  }
}
