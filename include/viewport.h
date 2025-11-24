#pragma once
#include <vector>
#include "shader.h"

class Viewport{
private:
    unsigned int width;
    unsigned int height;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int VAO;
    unsigned int accumulationSSBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
public:
    Shader* shader;
public:
    Viewport(unsigned int w, unsigned int h);
    void initialize();
    void draw();
    void resetAccumulation();
    void updateResolution(unsigned int w, unsigned int h);
    void createSSBO();
};
