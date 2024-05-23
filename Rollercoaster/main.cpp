// main file for openGL setup
// scene components encapsulated within their own objects

#include "headers.h"
#include "shMem.h"
#include "world.h"

// Error callback
void errorCallback( int error, const char* description ) {
    cerr << "Error " << error << ": " << description << endl;
    exit(1);
}


// Callbacks for when window size changes
void windowReshapeCallback( GLFWwindow* window, int width, int height ) {
    windowWidth = width;
    windowHeight = height;
}

void framebufferReshapeCallback( GLFWwindow* window, int width, int height ) {
    glViewport( 0, 0, width, height );
}

int main() {
  // Initialize the window

  glfwSetErrorCallback( errorCallback );

  GLFWwindow* window;

  if (!glfwInit())
    return 1;

#ifdef __APPLE__
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
  glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#else
  glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
#endif

  window = glfwCreateWindow( windowWidth, windowHeight, "Rollercoaster", NULL, NULL);

  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent( window );
  glfwSwapInterval( 1 );
  gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );

  glfwSetWindowSizeCallback( window, windowReshapeCallback );
  glfwSetFramebufferSizeCallback( window, framebufferReshapeCallback );

  // Set up the scene (and event handlers for mouse and keyboard)
  std::shared_ptr<World> world(new World( window ));

  // Some basic objects
  initSharedObjects();

  // Main loop

  struct timeb prevTime, thisTime; // record the last rendering time
  ftime( &prevTime );

  while (!glfwWindowShouldClose( window )) {

    // Update the world state

    ftime( &thisTime );
    float elapsedSeconds = (thisTime.time + thisTime.millitm / 1000.0) - (prevTime.time + prevTime.millitm / 1000.0);
    prevTime = thisTime;

    world->update( elapsedSeconds );
    world->draw( false ); // false = draw normally

    glfwPollEvents();
  }

  // Clean up

  glfwDestroyWindow( window );
  glfwTerminate();

  return 0;
}
