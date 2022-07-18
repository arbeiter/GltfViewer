### Performance fixes
- Extract stuff out into a custom mesh
- Extract texture out into a custom texture
- Render stuff in the main loop

### Checklist before git push

- Add Imgui to circle between all models
- First person camera
- Figure out how to input a metallic texture
- Test Non-PBR materials

### Done
- Test Duck.gltf
- Input entry modification
- Set default values if not present to the frag shader
  - Test1
  - Metallic etc.
- Test 4 and 5 one after the other
- Test Sponza first

### Notes

```
  class InternalModel {
    tinygltf::Model model;
    std::pair<GLuint, std::map<int, GLuint>> vaoAndEbos;
    std::vector<GLuint> allTextures;
  };

  Loader.loadTriangle(view) {
    // model
  };
  drawScene
loadTriangle(model)

```
