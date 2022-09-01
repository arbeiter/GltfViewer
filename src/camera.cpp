#include <camera.h>

glm::mat4 Camera::getViewMatrix() {
  glm::mat4 View = glm::mat4_cast(glm::conjugate(orientation));
  View[3][0] = -(View[0][0] * Position.x + View[1][0] * Position.y + View[2][0] * Position.z);
  View[3][1] = -(View[0][1] * Position.x + View[1][1] * Position.y + View[2][1] * Position.z);
  View[3][2] = -(View[0][2] * Position.x + View[1][2] * Position.y + View[2][2] * Position.z);

  return View;
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
  upAngle = upAngle < -90 ? -90 : upAngle;
  glm::quat aroundX = glm::angleAxis(glm::radians(-upAngle), glm::vec3(1,0,0));
  orientation = aroundY * aroundX;
  glm::quat qF = orientation * glm::quat(0,0,0,-1) * glm::conjugate(orientation);
  front = { qF.x, qF.y, qF.z };
  right = glm::normalize(glm::cross(front, glm::vec3(0,1,0)));
}
