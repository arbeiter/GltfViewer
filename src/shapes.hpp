class Shape
{
    public:
        std::string shapeType;
        GLfloat shapeAngle;
        glm::vec3 shapePosition;
        glm::vec3 shapeRotationAxis;
        glm::vec3 shapeScale;
        GLuint shapeVAO, shapeVBO, shapeEBO;

        Shape();
        ~Shape();
        void drawShape(Shader& lightingShader, glm::mat4& view, glm::mat4& projection, Camera& camera);
        void drawShape();
        void setShape(std::string type, glm::vec3 position);
        void setShapePosition(glm::vec3 position);
        void setShapeAngle(GLfloat angle);
        void setShapeScale(glm::vec3 scale);
};
