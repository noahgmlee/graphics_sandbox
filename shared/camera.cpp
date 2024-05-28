#include "camera.h"

void Camera::setV( vec3 initEyePosition, vec3 initEyeLookat, vec3 initEyeUp ) {
    distToCentre = (initEyeLookat - initEyePosition).length();

    // Compute initial world-to-view transform
    vec3 z = (initEyePosition - initEyeLookat).normalize();
    vec3 x = (initEyeUp ^ z).normalize();
    vec3 y = (z ^ x).normalize();

    V.rows[0] = vec4( x, -1 * initEyePosition * x );
    V.rows[1] = vec4( y, -1 * initEyePosition * y );
    V.rows[2] = vec4( z, -1 * initEyePosition * z );
    V.rows[3] = vec4( 0, 0, 0, 1 );
}

vec3 Camera::viewDirection() {
    return vec3( -V.rows[2].x, -V.rows[2].y, -V.rows[2].z );
}

// Upon a mouse press, record the current state (V) and this initial
// mouse position.
void Camera::mousePress( int button, float x, float y ) {
    currentButton = button;
    initMousePos = normalizeMousePos( x, y );
    initV = V;
}

// Upon a mouse drag, find the offset from the initial mouse position
// and calculate the corresponding change in V.
void Camera::mouseDrag( float x, float y ) {
    vec2 mousePos = normalizeMousePos(x,y);
    vec3 fromDir( initMousePos.x, initMousePos.y, 1 );
    vec3 toDir( mousePos.x, mousePos.y, 1 );
    float angle;
    vec3 axis;
    getRotation( fromDir, toDir, angle, axis );
    V = translate( vec3(0,0,-distToCentre) ) * rotate( angle, axis ) * translate( vec3(0,0,distToCentre) ) * initV;
}


// Upon a mouse scroll, translate in the view direction.
//
// Tranlate the view position forward or backward along the z axis by
// an amount proportional to the distance to the centre point.
void Camera::mouseScroll( float xoffset, float yoffset ) {
    const float factor = 1.1;
    float newDistance = (yoffset < 0) ? distToCentre/factor : distToCentre*factor;
    V = translate( 0, 0, (distToCentre - newDistance) ) * V;
    distToCentre = newDistance;
}



// Convert mouse position to [-1,1]x[-1,1]
vec2 Camera::normalizeMousePos( float x, float y ) {
    int width, height;
    glfwGetWindowSize( window, &width, &height );
    float w = width/2.0;
    float h = height/2.0;
    return vec2( (x - w)/w, (h - y)/h ); // flip in y direction
}

// Get the rotation between two directions in angle/axis form
void Camera::getRotation( vec3 from, vec3 to, float &angle, vec3 &axis ) {
    vec3 cross = from ^ to;
    angle = asin( cross.length() / (from.length()*to.length()) );
    axis = cross.normalize();
}
