#ifndef WORLD_H
#define WORLD_H

#include "headers.h"
#include "terrain.h" //reference from class work
#include "arcball.h"
#include "ctrlPoints.h"
#include "spline.h"
#include "train.h"
#include "cubeMap.h"

#define TRACK_PIECES_PER_SEG  20

#define POST_COLOUR vec3(0.8,0.9,0.5)


class World {
public:
    World( GLFWwindow *w );

    ~World() {
        delete terrain;
        delete spline;
        delete ctrlPoints;
        delete cubemap;
        delete train;
        delete arcball;
        delete gpu;
    }

    void draw( bool useItemTags );

    void mouseScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
    void mouseMovementCallback( GLFWwindow* window, double xpos, double ypos );
    void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
    void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );

    void mouseClick( vec3 v, int keyModifiers );

    void getMouseRay( int mouseX, int mouseY, vec3 &rayStart, vec3 &rayDir );

    void drawAllTrack( const mat4 &MV, const mat4 &MVP, vec3 lightDir );

    void update( float elapsedSeconds ) {
        if (ctrlPoints->count() > 1 && !pause)
            train->advance( elapsedSeconds );
    }

private:
    Terrain    *terrain;
    Spline     *spline;
    CtrlPoints *ctrlPoints;
    Train      *train;
    Arcball    *arcball;
    CubeMap    *cubemap;
    GPUProgram *gpu;

    GLFWwindow *window;

    mat4       VCStoCCS;
    float      fovy;

    // user-settable flags

    bool       drawTrack;
    bool       drawCoaster;
    bool       useArcLength;
    bool       showAxes;
    bool       drawUndersideOnly;
    bool       debug;
    bool       pause;
    bool       flag;
    bool       trainView; // to toggle train view
    bool       doRead;

    // for mouse picking and dragging:

    bool       arcballActive;
    bool       dragging;
    bool       draggingWasDone;

    vec3       startMousePos;
    vec3       startCtrlPointPos;
    int        selectedCtrlPoint;
    bool       movingSelectedBase;
};

#endif //WORLD_H
