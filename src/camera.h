#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
enum Camera_Movement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

class Camera
{

  public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 up;
    glm::vec3 WorldUp;
    glm::vec3 front;
    glm::vec3 right;
    float upAngle;
    float rightAngle;
    glm::quat orientation;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;

    Camera(glm::vec3 position) {
      Position = position;
      orientation = glm::quat(0, 0, 0, -1);
      rightAngle = 0.f;
      upAngle = 0.f;
      MovementSpeed = 10.0f;
      MouseSensitivity = 0.1f;
      updateCameraVectors();
    }
    glm::mat4 getViewMatrix();
    void processKeyboard(Camera_Movement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);
    //void ProcessMouseScroll(float yoffset);
    void updateCameraVectors();
};
