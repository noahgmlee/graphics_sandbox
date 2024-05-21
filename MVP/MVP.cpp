#include <ostream>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "headers.h"
#include "gpuProgram.h"

GPUProgram *myGPUProgram;
GLuint squareVAO; // A VAO for the cube
#define ROTATION_RATE (45.0/180.0*3.14159)

// The world state
struct worldState {
    float len;    // side length
    float theta;  // angle in radians
} state = { 0.5, 0.0 };

//key callback
void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods ) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose( window, GL_TRUE );
}

void errorCallback( int error, const char* description )
{
    std::cerr << "Error " << error << ": " << description << std::endl;
    exit(1);
}

void setupSquareVAO()
{
    glGenVertexArrays( 1, &squareVAO );
    glBindVertexArray( squareVAO );

    vec3 buffer[72];
    vec3* verticeBuffer = &buffer[0];
    vec3* colourBuffer = &buffer[36];

    //Triangle1
    verticeBuffer[0] = vec3( -0.5, -0.5, -0.5 ); // v0
    verticeBuffer[1] = vec3(  0.5, -0.5, -0.5 ); // v1
    verticeBuffer[2] = vec3( -0.5,  0.5, -0.5 ); // v2

    //Triangle2
    verticeBuffer[3] = vec3( -0.5,  0.5, -0.5 ); // v0
    verticeBuffer[4] = vec3(  0.5, -0.5, -0.5 ); // v1
    verticeBuffer[5] = vec3(  0.5,  0.5, -0.5 ); // v2

    //Triangle3
    verticeBuffer[6] = vec3( -0.5, -0.5, 0.5 ); // v0
    verticeBuffer[7] = vec3(  0.5, -0.5, 0.5  ); // v1
    verticeBuffer[8] = vec3( -0.5,  0.5, 0.5  ); // v2

    //Triangle4
    verticeBuffer[9] = vec3( -0.5,  0.5, 0.5  ); // v0
    verticeBuffer[10] = vec3(  0.5, -0.5, 0.5  ); // v1
    verticeBuffer[11] = vec3(  0.5,  0.5, 0.5  ); // v2

    //Triangle5
    verticeBuffer[12] = vec3( -0.5, -0.5, -0.5 ); // v0
    verticeBuffer[13] = vec3( -0.5, -0.5, 0.5 ); // v1
    verticeBuffer[14] = vec3( -0.5,  0.5, 0.5 ); // v2

    //Triangle6
    verticeBuffer[15] = vec3( -0.5,  0.5, -0.5 ); // v0
    verticeBuffer[16] = vec3( -0.5,  0.5, 0.5 ); // v1
    verticeBuffer[17] = vec3( -0.5,  -0.5, -0.5 ); // v2

    //Triangle7
    verticeBuffer[18] = vec3(  0.5, -0.5, -0.5 ); // v0
    verticeBuffer[19] = vec3(  0.5, -0.5, 0.5 ); // v1
    verticeBuffer[20] = vec3(  0.5,  0.5, 0.5 ); // v2

    //Triangle8
    verticeBuffer[21] = vec3(  0.5,  0.5, -0.5 ); // v0
    verticeBuffer[22] = vec3(  0.5,  0.5, 0.5 ); // v1
    verticeBuffer[23] = vec3(  0.5,  -0.5, -0.5 ); // v2

    //Triangle9
    verticeBuffer[24] = vec3( -0.5,  -0.5, -0.5 ); // v0
    verticeBuffer[25] = vec3( -0.5, -0.5, 0.5  ); // v1
    verticeBuffer[26] = vec3(  0.5, -0.5, 0.5  ); // v2

    //Triangle10
    verticeBuffer[27] = vec3(  0.5,  -0.5, -0.5  ); // v0
    verticeBuffer[28] = vec3(  0.5, -0.5, 0.5  ); // v1
    verticeBuffer[29] = vec3(  -0.5, -0.5, -0.5  ); // v2

    //Triangle11
    verticeBuffer[30] = vec3( -0.5,  0.5, -0.5  ); // v0
    verticeBuffer[31] = vec3( -0.5, 0.5, 0.5  ); // v1
    verticeBuffer[32] = vec3(  0.5, 0.5, 0.5  ); // v2

    //Triangle12
    verticeBuffer[33] = vec3( 0.5,  0.5, -0.5  ); // v0
    verticeBuffer[34] = vec3(  0.5, 0.5, 0.5   ); // v1
    verticeBuffer[35] = vec3(  -0.5, 0.5, -0.5 ); // v2

    //face1
    colourBuffer[0] = vec3( 0.1, 0.3, 0.8 ); // v0
    colourBuffer[1] = vec3( 0.1, 0.3, 0.8 ); // v1
    colourBuffer[2] = vec3( 0.1, 0.3, 0.8 ); // v2
    colourBuffer[3] = vec3( 0.1, 0.3, 0.8 ); // v0
    colourBuffer[4] = vec3( 0.1, 0.3, 0.8 ); // v1
    colourBuffer[5] = vec3( 0.1, 0.3, 0.8 ); // v2

    //face2
    colourBuffer[6] = vec3( 1.0, 1.0, 1.0 ); // v0
    colourBuffer[7] = vec3( 1.0, 1.0, 1.0 ); // v1
    colourBuffer[8] = vec3( 1.0, 1.0, 1.0 ); // v2
    colourBuffer[9] =  vec3( 1.0, 1.0, 1.0 ); // v0
    colourBuffer[10] = vec3( 1.0, 1.0, 1.0 ); // v1
    colourBuffer[11] = vec3( 1.0, 1.0, 1.0 ); // v2

    //face3
    colourBuffer[12] = vec3( .94, .89, .26 ); // v0
    colourBuffer[13] = vec3( .94, .89, .26 ); // v1
    colourBuffer[14] = vec3( .94, .89, .26 ); // v2
    colourBuffer[15] = vec3( .94, .89, .26 ); // v0
    colourBuffer[16] = vec3( .94, .89, .26 ); // v1
    colourBuffer[17] = vec3( .94, .89, .26 ); // v2

    //face4
    colourBuffer[18] = vec3(  .00, .45, .70 ); // v0
    colourBuffer[19] = vec3(  .00, .45, .70 ); // v1
    colourBuffer[20] = vec3(  .00, .45, .70 ); // v2
    colourBuffer[21] = vec3(  .00, .45, .70 ); // v0
    colourBuffer[22] = vec3(  .00, .45, .70 ); // v1
    colourBuffer[23] = vec3(  .00, .45, .70 ); // v2

    //face5
    colourBuffer[24] = vec3( .33, .70, .80 ); // v0
    colourBuffer[25] = vec3( .33, .70, .80 ); // v1
    colourBuffer[26] = vec3( .33, .70, .80 ); // v2
    colourBuffer[27] = vec3( .33, .70, .80 ); // v0
    colourBuffer[28] = vec3( .33, .70, .80 ); // v1
    colourBuffer[29] = vec3( .33, .70, .80 ); // v2

    //face6 -> interpolate
    colourBuffer[30] = vec3( 0.5, 0.5, 0.5 ); // v0
    colourBuffer[31] = vec3( 0.5, 0.5, 0.5 ); // v1
    colourBuffer[32] = vec3( 0.5, 0.5, 0.5 ); // v2
    colourBuffer[33] = vec3( 0.5, 0.5, 0.5 ); // v0
    colourBuffer[34] = vec3( 0.5, 0.5, 0.5 ); // v1
    colourBuffer[35] = vec3( 0.5, 0.5, 0.5 ); // v2

    GLuint VBO;
    glGenBuffers( 1, &VBO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW );

    // Attribute 0 is position
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 );

    // Attribute 1 is colour
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*) (3 * 3 * 12 * sizeof(float)) );
    glEnableVertexAttribArray( 1 );

    // Stop using this VBO.

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
}

void display()
{

    mat4 S = scale( state.len, state.len, state.len);   // scale by square size
    mat4 R = rotate( state.theta, vec3(1,1,1) ); // rotate by theta radians about (0,0,1)
    mat4 T = translate( vec3( -0.5, 0.5, 0 ) ); // translate from the origin to (-0.5,0.5,0)

    mat4 M = T * R * S;

    // vec3 eyePos = vec3(0.0, 0.0, 1.0);
    // mat4 MV = translate(-1 * eyePos) * M;


    myGPUProgram->activate();

    // Provide M to the vertex shader
    glUniformMatrix4fv( glGetUniformLocation( myGPUProgram->id(), "M"), 1, GL_TRUE, &M[0][0] );

    // Use the VAO that was set up above
    glBindVertexArray( squareVAO );
    glDrawArrays( GL_TRIANGLES, 0, 12 * 3);
    glBindVertexArray( 0 );

    myGPUProgram->deactivate();
}

void updateState( float elapsedSeconds )
{
    // increment angle
    state.theta += ROTATION_RATE * elapsedSeconds;
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

    myGPUProgram = new GPUProgram( "MVP/shader.vert", "MVP/shader.frag", "ProgramShader");
    myGPUProgram->activate();

    setupSquareVAO();

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

    glfwDestroyWindow( window );
    glfwTerminate();
    return 0;
}