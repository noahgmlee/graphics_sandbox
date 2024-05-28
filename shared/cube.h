// cylinder.h
//
// cylinder with radius 1 in xy plane and height 1 in z direction (between -0.5 and +0.5)


#ifndef CUBE_H
#define CUBE_H


#include "linalg.h"
#include "seq.h"
#include "gpuProgram.h"

class Cube {

public:

    Cube() {

        // Verticies
        // Top Face
        verts.add(vec3(-0.5f, 0.5f, -0.5f));
        verts.add(vec3(0.5f, 0.5f, -0.5f));
        verts.add(vec3(-0.5f, 0.5f, 0.5f));
        verts.add(vec3(0.5f, 0.5f, 0.5f));

        // Bottom face
        verts.add(vec3(-0.5f, -0.5f, 0.5f));
        verts.add(vec3(0.5f, -0.5f, 0.5f));
        verts.add(vec3(-0.5f, -0.5f, -0.5f));
        verts.add(vec3(0.5f, -0.5f, -0.5f));

        // Front Face - good
        verts.add(vec3(-0.5f, 0.5f, 0.5f));
        verts.add(vec3(0.5f, 0.5f, 0.5f));
        verts.add(vec3(-0.5f, -0.5f, 0.5f));
        verts.add(vec3(0.5f, -0.5f, 0.5f));

        // Back Face
        verts.add(vec3(-0.5f, -0.5f, -0.5f));
        verts.add(vec3(0.5f, -0.5f, -0.5f));
        verts.add(vec3(-0.5f, 0.5f, -0.5f));
        verts.add(vec3(0.5f, 0.5f, -0.5f));

        // Left Face
        verts.add(vec3(-0.5f, 0.5f, -0.5f));
        verts.add(vec3(-0.5f, 0.5f, 0.5f));
        verts.add(vec3(-0.5f, -0.5f, -0.5f));
        verts.add(vec3(-0.5f, -0.5f, 0.5f));

        // Right Face
        verts.add(vec3(0.5f, 0.5f, 0.5f));
        verts.add(vec3(0.5f, 0.5f, -0.5f));
        verts.add(vec3(0.5f, -0.5f, 0.5f));
        verts.add(vec3(0.5f, -0.5f, -0.5f));

        // Normals
        // Top Face
        normals.add(vec3(0, 1, 0));
        normals.add(vec3(0, 1, 0));
        normals.add(vec3(0, 1, 0));
        normals.add(vec3(0, 1, 0));

        // Bottom Face
        normals.add(vec3(0, -1, 0));
        normals.add(vec3(0, -1, 0));
        normals.add(vec3(0, -1, 0));
        normals.add(vec3(0, -1, 0));

        // Front Face
        normals.add(vec3(0, 0, 1));
        normals.add(vec3(0, 0, 1));
        normals.add(vec3(0, 0, 1));
        normals.add(vec3(0, 0, 1));

        // Back Face
        normals.add(vec3(0, 0, -1));
        normals.add(vec3(0, 0, -1));
        normals.add(vec3(0, 0, -1));
        normals.add(vec3(0, 0, -1));

        // Left Face
        normals.add(vec3(-1, 0, 0));
        normals.add(vec3(-1, 0, 0));
        normals.add(vec3(-1, 0, 0));
        normals.add(vec3(-1, 0, 0));

        // Right Face
        normals.add(vec3(1, 0, 0));
        normals.add(vec3(1, 0, 0));
        normals.add(vec3(1, 0, 0));
        normals.add(vec3(1, 0, 0));

        gpu.init(vertShader, fragShader, "in cube.cpp");

        setupVAO();
    };

    ~Cube() {}

    // The cube drawing provides to the shader (a) the OCS vertex
    // position as attribute 0 and (b) the OCS vertex normal as
    // attribute 1.
    //
    // The cube is drawn at the origin with side length 1
    // (in [-0.5,+0.5]).  You must provide mat4 MV and mat4 MVP matrices,
    // as well as vec3 lightDir and vec3 colour.

    void draw(const mat4& MV, const mat4& MVP, vec3 lightDir, vec3 colour);

private:

    seq<vec3>         verts;
    seq<vec3>         normals;
    GLuint            VAO;

    GPUProgram        gpu;

    static const char* vertShader;
    static const char* fragShader;

    void setupVAO();
};

#endif