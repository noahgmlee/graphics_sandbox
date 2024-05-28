// camera.h
// provides the V matrix for world rendering


#ifndef OBSERVER_H
#define OBSERVER_H

#include "headers.h"
#include "linalg.h"


class Camera {
public:
    Camera( GLFWwindow *w ) {
        window = w;
    }

    Camera( GLFWwindow *w, vec3 initEyePosition, vec3 initEyeLookat, vec3 initEyeUp ) {
        window = w;
        setV( initEyePosition, initEyeLookat, initEyeUp );
    }

    void setV( vec3 initEyePosition, vec3 initEyeLookat, vec3 initEyeUp );

    mat4 getV() {
        return V;
    }
    vec3 viewDirection();

    void mousePress( int button, float x, float y );
    void mouseDrag( float x, float y );
    void mouseScroll( float xoffset, float yoffset );

private:
    vec2 normalizeMousePos( float x, float y );
    void getRotation( vec3 from, vec3 to, float &angle, vec3 &axis );

    float distToCentre; // distance to the 'look at' point
    mat4 V;  // world-to-view transform
    GLFWwindow *window;

    int currentButton;  // button down during drag
    vec2 initMousePos;  // initial (x,y) at button down
    mat4 initV;         // initial V at button down
};

#endif
