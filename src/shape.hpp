#ifndef SHAPE_HPP
#define SHAPE_HPP

class Shape {
  public:
    Shape();
    Mesh createCubeMesh(const glm::vec3& dimensions);
    void setupShader();
}

#endif
