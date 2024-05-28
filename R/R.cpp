#include <ostream>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "headers.h"
#include "gpuProgram.h"
#include "axes.h"
#include "axis.h"
#include "sphere.h"
#include "plane.h"

// world objects
Axes* axes;
Axis* axis;
Sphere* sphere;
Plane* plane;

// The world state
struct worldState {
    mat4  worldV;
    mat4  worldP;
    mat4  MVPaxes;
    mat4  MVPaxis;
    mat4  MVPplane;
    mat4  Tsphere;
    mat4  Ssphere;
    mat4  MVPsphere;
    float theta;  // angle in radians
    bool  pause;
    bool  drawPlane;
    worldState() {
        worldV = identity4();
        worldP = identity4();
        MVPaxes = identity4();
        MVPaxis = identity4();
        MVPplane = identity4();
        Tsphere = identity4();
        Ssphere = identity4();
        MVPsphere = identity4();
        theta = 0.0;
        pause = true;
        drawPlane = false;
    }
} state;

//key callback
void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods ) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose( window, GL_TRUE );
    if (key == 'P' && action == GLFW_PRESS)
        state.pause = !state.pause;
    if (key == 'D' && action == GLFW_PRESS)
        state.drawPlane = !state.drawPlane;
}

void errorCallback( int error, const char* description )
{
    std::cerr << "Error " << error << ": " << description << std::endl;
    exit(1);
}

void updateState( float elapsedSeconds )
{
    if (!state.pause) {
        // increment angle
        state.theta += M_PI/4.0 * elapsedSeconds; // 1/4th PI radians per s
        mat4 R = rotate(state.theta, {0.1, 0.1, -0.1});
        state.MVPsphere = state.worldP * state.worldV * R * state.Tsphere * state.Ssphere;
    }
}

mat4 initV(vec3 initEyePosition, vec3 initEyeLookat, vec3 initEyeUp) {
    // Compute initial world-to-view transform
    vec3 z = (initEyePosition - initEyeLookat).normalize();
    vec3 x = (initEyeUp ^ z).normalize();
    vec3 y = (z ^ x).normalize();

    mat4 V;

    V.rows[0] = vec4( x, -1 * initEyePosition * x );
    V.rows[1] = vec4( y, -1 * initEyePosition * y );
    V.rows[2] = vec4( z, -1 * initEyePosition * z );
    V.rows[3] = vec4( 0, 0, 0, 1 );
    return V;
}

void display() {
    axes->draw(state.MVPaxes);
    axis->draw(state.MVPaxis);
    sphere->draw(state.Ssphere, state.MVPsphere, {0.25, 0.1, 1.0}, {1.0, 1.0, 1.0});
    if (state.drawPlane) {
        //draw plane
        plane->draw(state.MVPplane);
    }
}

int main() {
    glfwSetErrorCallback( errorCallback );

    glfwInit();

    //version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // MACOS requirement
#else
    glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval( 1 );
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSetKeyCallback( window, keyCallback );

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    mat4 axesM = scale(0.1, 0.1, 0.1);
    mat4 axisM = scale(0.1, 0.1, 0.1);
    state.Tsphere  = translate(0.05, 0.05, -0.1);
    state.Ssphere = scale(0.01, 0.01, 0.01);
    state.worldV = initV({0.1, 0.0, 1}, {0, 0, 0}, {0, 1, 0});
    state.worldP = perspective(30.0 * (M_PI/180.0), 800.0/600.0, 0.1, 100);
    state.MVPaxes = state.worldP * state.worldV * axesM;
    state.MVPaxis = state.worldP * state.worldV * axisM;
    state.MVPsphere = state.worldP * state.worldV * state.Tsphere * state.Ssphere;
    axes = new Axes();
    axis = new Axis({1, 1, -1});
    sphere = new Sphere(4);
    vec3 b1 = (vec3({1, 1, -1}) ^ vec3(0.05, 0.05, -0.1)).normalize();
    vec3 b2 = (vec3({1, 1, -1}) ^ b1).normalize();
    vec3 offset = vec3(0.1, 0.1, -0.1).normalize() * vec3(0.5, 0.5, -1) * vec3(0.1, 0.1, -0.1).normalize();
    plane = new Plane(b1.normalize(), b2.normalize());
    mat4 Tplane = translate(-0.5 * b1 - 0.5 * b2 + offset);
    state.MVPplane = state.worldP * state.worldV * scale(0.1, 0.1, 0.1) * Tplane;

    struct timeb prevTime, thisTime; // record the last rendering time
    ftime( &prevTime );

    while(!glfwWindowShouldClose(window))
    {
        ftime( &thisTime );
        float elapsedSeconds =   (thisTime.time + thisTime.millitm / 1000.0)
                            - (prevTime.time + prevTime.millitm / 1000.0);
        prevTime = thisTime;

        // Update the world state

        updateState( elapsedSeconds );

        glClearColor( 0.0, 0.0, 0.0, 0.0 );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        display();
        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    delete axes;
    delete axis;
    delete sphere;
    delete plane;
    glfwDestroyWindow( window );
    glfwTerminate();
    return 0;
}