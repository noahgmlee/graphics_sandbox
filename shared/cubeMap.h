#ifndef CUBE_MAP_H
#define CUBE_MAP_H


#include "linalg.h"
#include "seq.h"
#include "gpuProgram.h"
#include <vector>

class CubeMap {

public:

    CubeMap() {

        cubemapTexture = loadCubemap(faces);

        gpu.init(vertShader, fragShader, "in cubeMap.cpp");

        setupVAO();
    };

    ~CubeMap() {}

    unsigned int loadCubemap(vector<std::string> faces);
    void draw(mat4& V, mat4& P);

private:
    vector<std::string> faces
    {
        "Rollercoaster/Textures/right.jpg",
        "Rollercoaster/Textures/left.jpg",
        "Rollercoaster/Textures/top.jpg",
        "Rollercoaster/Textures/bottom.jpg",
        "Rollercoaster/Textures/front.jpg",
        "Rollercoaster/Textures/back.jpg"
    };

    unsigned int cubemapTexture;

    GLfloat      verts[108] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    GLuint            VAO;
    GLuint            VBO;

    GPUProgram        gpu;

    static const char* vertShader;
    static const char* fragShader;

    void setupVAO();
};

#endif