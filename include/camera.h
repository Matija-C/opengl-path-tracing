#pragma once
#include "glm/glm.hpp"

const float PI = 3.14159265;

class Camera{
public:
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;
    float pitch;
    float yaw;
public:
    Camera();
    void updateDirections();
    void updateOrientation(float deltaX, float deltaY);
    void moveForward(float delta);
    void moveBackward(float delta);
    void moveLeft(float delta);
    void moveRight(float delta);
    void moveUpward(float delta);
    void moveDownward(float delta);
};
