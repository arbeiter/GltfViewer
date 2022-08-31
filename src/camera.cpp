#include <camera.h>

glm::mat4 Camera::getViewMatrix() {
  glm::quat reverseOrient = glm::conjugate(orientation);
  glm::mat4 rot = glm::mat4_cast(reverseOrient);
  glm::mat4 translation = glm::translate(glm::mat4(1.0), -Position);
  return rot * translation;
}

void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
  float velocity = MovementSpeed * deltaTime;

  if (direction == Camera_Movement::FORWARD)
   Position += front * velocity;

  if (direction == Camera_Movement::BACKWARD)
   Position -= front * velocity;

  if (direction == Camera_Movement::LEFT)
   Position -= right * velocity;

  if (direction == Camera_Movement::RIGHT)
   Position += right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset) {
  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;
  rightAngle += xoffset;
  upAngle += yoffset;
  updateCameraVectors();
}

void Camera::updateCameraVectors() {
  glm::quat aroundY = glm::angleAxis(glm::radians(-rightAngle), glm::vec3(0,1,0));
  glm::quat aroundX = glm::angleAxis(glm::radians(upAngle), glm::vec3(1,0,0));
  orientation = aroundY * aroundX;
  glm::quat qF = orientation * glm::quat(0,0,0,-1) * glm::conjugate(orientation);
  front = { qF.x, qF.y, qF.z };
  right = glm::normalize(glm::cross(front, glm::vec3(0,1,0)));
}
