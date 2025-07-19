#pragma once
#include <vector>
#include "shader.h"

class Viewport{
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int VAO;
public:
    Shader* shader;
public:
    void initialize();
    void draw();
};
