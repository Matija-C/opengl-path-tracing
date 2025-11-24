#include <vector>
#include "viewport.h"
#include "shader.h"

Viewport::Viewport(unsigned int w, unsigned int h)
        : width(w), height(h) {}

void Viewport::initialize(){
    vertices = {
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f 
    };

    indices = {
        0, 1, 3,
        1, 2, 3
    };

    shader = new Shader("src/viewportVertex.glsl", "src/viewportFragment.glsl"); 

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    createSSBO();
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);
}

void Viewport::createSSBO(){
    if(accumulationSSBO != 0){
        glDeleteBuffers(1, &accumulationSSBO);
    }
    
    glGenBuffers(1, &accumulationSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, accumulationSSBO);
    size_t bufferSize = width * height * sizeof(float) * 4;
    std::vector<float> initialData(width * height * 4, 0.0f);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, initialData.data(), GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Viewport::updateResolution(unsigned int w, unsigned int h){
    width = w;
    height = h;

    createSSBO();
}

void Viewport::resetAccumulation(){
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, accumulationSSBO);
    std::vector<float> black(width * height * 4, 0.0);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, width * height * sizeof(float) * 4, black.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Viewport::draw(){
    shader->use();
    shader->setInt("viewportWidth", width);
    shader->setInt("viewportHeight", height);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, accumulationSSBO);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
