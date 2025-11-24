#include "glm/glm.hpp"
#include "camera.h"
#include <cmath>
#include <iostream>

Camera::Camera()
    : position(glm::vec3(0.0f, 0.0f, 0.0f)), forward(glm::vec3(0.0, 0.0, -1.0)),
      right(glm::vec3(1.0, 0.0, 0.0)), up(glm::vec3(0.0, 1.0, 0.0)), pitch(0.0f), yaw(0.0f){}
void Camera::updateDirections(){
    glm::vec3 newForward;
    newForward.x = cos(pitch) * cos(yaw);
    newForward.y = sin(pitch);
    newForward.z = cos(pitch) * sin(yaw);

    forward = glm::normalize(newForward);
    right = glm::normalize(glm::cross(forward, glm::vec3(0.0, 1.0, 0.0)));
    up = glm::normalize(glm::cross(right, forward));
}
void Camera::updateOrientation(float deltaX, float deltaY){
    yaw -= deltaX * 0.01f;
    pitch += deltaY * 0.01f;

    if(pitch >  glm::radians(89.0f)) pitch =  glm::radians(89.0f);
    if(pitch < -glm::radians(89.0f)) pitch = -glm::radians(89.0f);

    updateDirections();
}
void Camera::moveForward(float delta){
    position -= forward * delta;
}
void Camera::moveBackward(float delta){
    position += forward * delta;
}
void Camera::moveLeft(float delta){
    position -= right * delta;
}
void Camera::moveRight(float delta){
    position += right * delta;
}
void Camera::moveUpward(float delta){
    position.y += delta;
}
void Camera::moveDownward(float delta){
    position.y -= delta;
}
