// frustum.h
//
// frustum interface to move viewpoint around.
//
// Drag mouse with left button to rotate axes
// Drag mouse with right button to reposition eye


#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "headers.h"
#include "linalg.h"
#include "gpuProgram.h"
#include "axes.h"

#define FRUSTUM_VERTICES 27

class Frustum {
public:
    Frustum( GLFWwindow *w, int width, int height) {
        window = w;
        this->width = width;
        this->height = height;
        axes = new Axes();
        gpu.init(vertShader, fragShader, "in frustum.cpp");
        setupVAO();
        Tinverse = identity4();
        T = identity4();
        R = identity4();
        S = scale(5 * (float)width/(float)height, 5, 10);
        M = T * R * S;
        thetaX = 0.0;
        thetaY = 0.0;
        nearY = 0.05;
        fovy = atan2(2*nearY, 0.1); //2*y because its the angle around the eye
    }

    ~Frustum() {
        delete axes;
    }

    void horizontalMove(bool dir); // true = right
    void verticalMove(bool dir); // true = up
    void horizontalRot(bool dir);
    void verticalRot(bool dir);
    void expandView(bool bigger);

    mat4 getM() { return M; }
    mat4 getV();
    mat4 getP();

    void draw(mat4& MV, mat4& MVP, mat4& axesMVP);

private:
    mat4 M, T, R, S, Tinverse;
    int width, height;
    float thetaX, thetaY;
    float nearY, fovy;

    Axes* axes;

    GLFWwindow *window;
    GPUProgram gpu;

    vec2 normalizeMousePos( float x, float y );

    static const char* vertShader;
    static const char* fragShader;

    vec3 verts[FRUSTUM_VERTICES] = {
        {0.0, 0.0, 0.0},
        {-0.01, -0.01, -0.01}, //l1

        {0.0, 0.0, 0.0},
        {-0.01, 0.01, -0.01}, //l3

        {0.0, 0.0, 0.0},
        {0.01, 0.01, -0.01}, //l5

        {0.0, 0.0, 0.0},
        {0.01, -0.01, -0.01}, //l7
        {-0.01, -0.01, -0.01}, //l8
        {-0.01, 0.01, -0.01}, //l9
        {0.01, 0.01, -0.01}, //l10
        {0.01, -0.01, -0.01}, //l11

        {-0.01, -0.01, -0.01},
        {-0.1, -0.1, -0.1}, //l13

        {-0.01, -0.01, -0.01},
        {-0.01, 0.01, -0.01},
        {-0.1, 0.1, -0.1}, //l16

        {-0.01, 0.01, -0.01},
        {0.01, 0.01, -0.01},
        {0.1, 0.1, -0.1}, //l19

        {0.01, 0.01, -0.01},
        {0.01, -0.01, -0.01},
        {0.1, -0.1, -0.1}, //l11

        {-0.1, -0.1, -0.1}, //l13
        {-0.1, 0.1, -0.1}, //l14
        {0.1, 0.1, -0.1}, //l15
        {0.1, -0.1, -0.1}, //l16
    };

    GLuint VAO;
    void setupVAO();
};

#endif // FRUSTUM_H
