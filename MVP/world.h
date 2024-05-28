#ifndef WORLD_H
#define WORLD_H

#include "headers.h"
#include "gpuProgram.h"
#include "camera.h"
#include "frustum.h"
#include "cubeMap.h"
#include "cube.h"
#include "sphere.h"

#define NUM_CUBES 5
#define NUM_SPHERES 5

struct cubeM {
    float rotationRate;
    float theta;
    mat4 R;
    mat4 S;
    mat4 T;
};

class World {
public:
    World( GLFWwindow *w, int windowWidth, int windowHeight );

    ~World() {
        delete gpu;
        delete camera;
        delete frustum;
        delete skybox;
        delete cube;
        delete sphere;
    }

    void draw( );

    void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );

    void mouseScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
    void mouseMovementCallback( GLFWwindow* window, double xpos, double ypos );
    void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods );

    void update( float elapsedSeconds ) {
        for (auto& c : cubes) {
            c.theta += elapsedSeconds * c.rotationRate;
        }
    }

private:
    GPUProgram* gpu;
    Camera*     camera;
    Frustum*    frustum;
    CubeMap*    skybox;

    Cube*       cube;
    Sphere*     sphere;

    cubeM cubes[NUM_CUBES];
    mat4 spheres[NUM_SPHERES];

    int width;
    int height;

    GLFWwindow *window;
    float      fovy;

    // user-settable flags
    bool       insideView; // to toggle frustum view
    bool       insideTransition; // smooth transition to frustum
    bool       outsideTransition; // smooth transition back to camera
    int        transitionTicks;

    mat4       gradientM(mat4& M1, mat4& M2);
    void       gradientP(mat4& M2);
    void       gradientV(mat4& M2);
    mat4       addM4(mat4& M1, mat4& M2);
    mat4       dV;
    mat4       dP;
    mat4       V;
    mat4       P;

    bool       drawSkybox; // to toggle a skybox
};

#endif //WORLD_H
