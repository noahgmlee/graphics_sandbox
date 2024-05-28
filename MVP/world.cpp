// persp.cpp

#include "headers.h"
#include "world.h"

#include <strstream>
#include <fstream>
#include <iomanip>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define LIGHT_DIR 1,1,3
#define TRANSITION_FRAMES 100.0

World::World( GLFWwindow *w, int windowWidth, int windowHeight ) {
    window = w;
    width = windowWidth;
    height = windowHeight;
    drawSkybox = false;
    insideTransition = false;
    outsideTransition = false;

    glfwSetWindowUserPointer( window, this );

    // Key handler

    auto static_keyCallback = [](GLFWwindow* ww, int key, int scancode, int action, int mods ) {
        static_cast<World*>(glfwGetWindowUserPointer(ww))->keyCallback( ww, key, scancode, action, mods );
    };

    glfwSetKeyCallback( window, static_keyCallback );

    // mouse handler

    auto static_mouseButtonCallback = [](GLFWwindow* ww, int button, int action, int keyModifiers ) {
        static_cast<World*>(glfwGetWindowUserPointer(ww))->mouseButtonCallback( ww, button, action, keyModifiers );
    };

    glfwSetMouseButtonCallback( window, static_mouseButtonCallback );

    auto static_mouseScrollCallback = [](GLFWwindow* ww, double xoffset, double yoffset ) {
        static_cast<World*>(glfwGetWindowUserPointer(ww))->mouseScrollCallback( ww, xoffset, yoffset );
    };

    glfwSetScrollCallback( window, static_mouseScrollCallback );

    // Set up camera interface

    camera = new Camera(window);
    camera->setV({0.1, 0.0, 1.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0});

    // Set up GPU program

    gpu = new GPUProgram( "MVP/Shaders/shader.vert", "MVP/Shaders/shader.frag", "shader files for world.cpp" );

    // Set up world

    skybox = new CubeMap();
    frustum = new Frustum( window, width, height );
    cube   = new Cube();
    sphere = new Sphere(4);

    // Miscellaneous stuff
    insideView = false;
    fovy = 30.0/180.0*M_PI;
    transitionTicks = 0;
    dV = identity4();
    dP = identity4();
    V = identity4();
    P = identity4();

    // seed random generator for object orientation
    uint64_t seed = static_cast<uint64_t>( time(NULL) );
    srand( seed );

    for (auto& s : spheres) {
        float x, y, z;
        float scaleFactor;
        x = ((double) rand() / (double) RAND_MAX) * 2.0 - 1.0;
        y = ((double) rand() / (double) RAND_MAX) * 2.0 - 1.0;
        z = ((double) rand() / (double) RAND_MAX) * 2.0 - 1.0;
        scaleFactor = ((double) rand() / (double) RAND_MAX) * 0.1; //max of 0.1
        s = translate({x, y, z}) * scale(scaleFactor, scaleFactor, scaleFactor);
    }

    for (auto& c : cubes) {
        float x, y, z;
        float scaleFactor;
        x = ((double) rand() / (double) RAND_MAX) * 2.0 - 1.0;
        y = ((double) rand() / (double) RAND_MAX) * 2.0 - 1.0;
        z = ((double) rand() / (double) RAND_MAX) * 2.0 - 1.0;
        scaleFactor = ((double) rand() / (double) RAND_MAX) * 0.1; //max of 0.01
        c.rotationRate = ((double) rand() / (double) RAND_MAX) * 2 * 3.14; //max 1 rotation per second
        c.theta = ((double) rand() / (double) RAND_MAX) * 2 * 3.14; //start anywhere on the 3D angle
        c.T = translate({x, y, z});
        c.R = rotate(c.theta, {1,1,1});
        c.S = scale(scaleFactor, scaleFactor, scaleFactor);
    }
}


void World::draw() {
    glClearColor( 0,0,0,0 );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_DEPTH_TEST );

    // Draw objects

    if (!insideView) { //inside perspective
        if (outsideTransition) {
            transitionTicks += 1;
            // run V transition first to avoid early clipping
            if (transitionTicks <= 1 * TRANSITION_FRAMES) {
                P = addM4(P, dP);
            } else {
                V = addM4(V, dV);
            }
            if (transitionTicks >= 2 * TRANSITION_FRAMES) {
                transitionTicks = 0;
                outsideTransition = false;
            }
        } else {
            V = camera->getV();
            P = perspective(fovy,
                            width/(float)height,
                            0.1,
                            100);
        }
        mat4 M = frustum->getM();
        mat4 MV = V * M;
        mat4 MVP = P * MV;
        mat4 axesM = frustum->getM() * scale(0.01 * height/width, 0.01, 0.01);
        mat4 axesMV = V * axesM;
        mat4 axesMVP = P * axesMV;
        frustum->draw(MV, MVP, axesMVP);
    } else {
        if (insideTransition) {
            transitionTicks += 1;
            // run V transition first to avoid early clipping
            if (transitionTicks <= 1 * TRANSITION_FRAMES) {
                V = addM4(V, dV);
            } else {
                P = addM4( P, dP);
            }
            if (transitionTicks >= 2 * TRANSITION_FRAMES) {
                transitionTicks = 0;
                insideTransition = false;
            }
        } else {
            V = frustum->getV();
            P = frustum->getP();
        }
    }

    for (auto& s : spheres) {
        mat4 MV = V * s;
        mat4 MVP = P * MV;
        sphere->draw(MV, MVP, {LIGHT_DIR}, {1.0, 1.0, 1.0});
    }

    for (auto& c : cubes) {
        c.R = rotate(c.theta, {1, 1, 1});
        mat4 M = c.T * c.R * c.S;
        mat4 MV = V * M;
        mat4 MVP = P * MV;
        cube->draw(MV, MVP, {LIGHT_DIR}, {1.0, 1.0, 1.0});
    }

    if (drawSkybox) {
        skybox->draw(V, P); // (cubemap shaders take only V and P)
    }

    glfwSwapBuffers( window );
}

mat4 World::gradientM(mat4& M1, mat4& M2) {
    mat4 res;
    for (int i = 0; i < 4; i++) {
        res.rows[i].x = M2.rows[i].x - M1.rows[i].x;
        res.rows[i].y = M2.rows[i].y - M1.rows[i].y;
        res.rows[i].z = M2.rows[i].z - M1.rows[i].z;
        res.rows[i].w = M2.rows[i].w - M1.rows[i].w;
    }
    return 1/TRANSITION_FRAMES * res;
}

void World::gradientV(mat4& M2) {
    dV = gradientM(V, M2);
}

void World::gradientP(mat4& M2) {
    dP = gradientM(P, M2);
}

mat4 World::addM4(mat4& M1, mat4& M2) {
    mat4 res;
    for (int i = 0; i < 4; i++) {
        res.rows[i].x = M2.rows[i].x + M1.rows[i].x;
        res.rows[i].y = M2.rows[i].y + M1.rows[i].y;
        res.rows[i].z = M2.rows[i].z + M1.rows[i].z;
        res.rows[i].w = M2.rows[i].w + M1.rows[i].w;
    }
    return res;
}

// Key callback
void World::keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods ){
    static bool uphold = false, downhold = false,
                lefthold = false, righthold = false,
                rotuphold = false, rotdownhold = false,
                rotlefthold = false, rotrighthold = false,
                fovyuphold = false, fovydownhold = false;
    if (action == GLFW_PRESS)
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose( window, GL_TRUE );
                break;

            case GLFW_KEY_RIGHT:
                righthold = true;
                break;

            case GLFW_KEY_LEFT:
                lefthold = true;
                break;

            case GLFW_KEY_UP:
                uphold = true;
                break;

            case GLFW_KEY_DOWN:
                downhold = true;
                break;

            case 'W':
                rotuphold = true;
                break;

            case 'A':
                rotlefthold = true;
                break;

            case 'S':
                rotdownhold = true;
                break;

            case 'D':
                rotrighthold = true;
                break;

            case '=':
                fovyuphold = true;
                break;

            case '-':
                fovydownhold = true;
                break;

            case 'V':
                insideView = !insideView;
                if (insideView) {
                    insideTransition = true;
                    outsideTransition = false;
                    mat4 V2 = frustum->getV();
                    mat4 P2 = frustum->getP();
                    gradientV(V2);
                    gradientP(P2);
                } else {
                    outsideTransition = true;
                    insideTransition = false;
                    mat4 V2 = camera->getV();
                    mat4 P2 = perspective(fovy,
                                          width/(float)height,
                                          0.1,
                                          100);
                    gradientV(V2);
                    gradientP(P2);
                }
                break;

            case 'C':
                drawSkybox = !drawSkybox;
                break;

            case '/':
                cout  << "arrows - move frustum" << endl;
                cout  << "'wasd' - rotate frustum" << endl;
                cout  << "'+' - increase FOVY" << endl;
                cout  << "'-' - decrease FOVY" << endl;
                cout  << "'v' - toggle internal viewPoint reference" << endl;
                break;

            default:
                cout  << "unbound key - toggle internal viewPoint reference" << endl;
                break;
    } else if (action == GLFW_RELEASE) {
        uphold = false, downhold = false,
        lefthold = false, righthold = false,
        rotuphold = false, rotdownhold = false,
        rotlefthold = false, rotrighthold = false,
        fovyuphold = false, fovydownhold = false;
    }
    if (uphold)
        frustum->verticalMove(true);
    if (downhold)
        frustum->verticalMove(false);
    if (righthold)
        frustum->horizontalMove(true);
    if (lefthold)
        frustum->horizontalMove(false);
    if (rotuphold)
        frustum->verticalRot(true);
    if (rotdownhold)
        frustum->verticalRot(false);
    if (rotrighthold)
        frustum->horizontalRot(true);
    if (rotlefthold)
        frustum->horizontalRot(false);
    if (fovyuphold)
        frustum->expandView(true);
    if (fovydownhold)
        frustum->expandView(false);
}

void World::mouseButtonCallback( GLFWwindow* window, int button, int action, int keyModifiers ) {
    // Get mouse position

    double xpos, ypos;
    glfwGetCursorPos( window, &xpos, &ypos );

    if (action == GLFW_PRESS) {

        // Handle mouse press

        // Start tracking the mouse

        auto static_mouseMovementCallback = [](GLFWwindow* window, double xpos, double ypos ) {
        static_cast<World*>(glfwGetWindowUserPointer(window))->mouseMovementCallback( window, xpos, ypos );
        };

        glfwSetCursorPosCallback( window, static_mouseMovementCallback );

        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            camera->mousePress( button, xpos, ypos );
        }
    } else {
        glfwSetCursorPosCallback( window, NULL ); // unregister callback on release
    }
}

void World::mouseMovementCallback( GLFWwindow* window, double xpos, double ypos ) {
    camera->mouseDrag( xpos, ypos );
}

void World::mouseScrollCallback( GLFWwindow* window, double xoffset, double yoffset ) {
    camera->mouseScroll( xoffset, yoffset ); // tell arcball about this
}
