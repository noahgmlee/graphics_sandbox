// persp.cpp


#include "headers.h"
#include "world.h"
#include "shMem.h"

#include <strstream>
#include <fstream>
#include <iomanip>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define LIGHT_DIR 1,1,3

World::World( GLFWwindow *w )

{
  window = w;

  glfwSetWindowUserPointer( window, this );

  // Key handler

  auto static_keyCallback = [](GLFWwindow* ww, int key, int scancode, int action, int mods ) {
    static_cast<World*>(glfwGetWindowUserPointer(ww))->keyCallback( ww, key, scancode, action, mods );
  };

  glfwSetKeyCallback( window, static_keyCallback );

  // Mouse callbacks

  auto static_mouseButtonCallback = [](GLFWwindow* ww, int button, int action, int keyModifiers ) {
    static_cast<World*>(glfwGetWindowUserPointer(ww))->mouseButtonCallback( ww, button, action, keyModifiers );
  };

  glfwSetMouseButtonCallback( window, static_mouseButtonCallback );

  auto static_mouseScrollCallback = [](GLFWwindow* ww, double xoffset, double yoffset ) {
    static_cast<World*>(glfwGetWindowUserPointer(ww))->mouseScrollCallback( ww, xoffset, yoffset );
  };

  glfwSetScrollCallback( window, static_mouseScrollCallback );

  // Set up arcball interface

  arcball = new Arcball( window );

  // Set up GPU program

  gpu = new GPUProgram( "Rollercoaster/Shaders/shader.vert", "Rollercoaster/Shaders/shader.frag", "shader files for world.cpp" );

  // Set up world

  spline     = new Spline();
  ctrlPoints = new CtrlPoints( spline, window );
  train      = new Train( spline );
  terrain    = new Terrain( string("Rollercoaster/Textures/"), "hills-heights.png", "hills-texture.png" );
  cubemap    = new CubeMap();

  // Miscellaneous stuff

  pause = false;
  dragging = false;
  arcballActive = false;
  drawTrack = true;
  drawCoaster = true;
  drawUndersideOnly = false;
  useArcLength = false;
  showAxes = false;
  debug = false;
  flag = false;
  trainView = false; // train view toggle flag
  fovy = 15/180.0*M_PI;
  doRead = true; // to return to initial view after train flag is toggled off
}


void World::draw( bool useItemTags )

{
  glClearColor( 0,0,0, 0 );

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable( GL_DEPTH_TEST );

  // model-view transform (i.e. OCS-to-VCS)

  float diag = sqrt( terrain->texture->width*terrain->texture->width + terrain->texture->height*terrain->texture->height );

  VCStoCCS = perspective( fovy,
                          windowWidth/(float)windowHeight,
                          MAX(0.1,arcball->distToCentre - 1.2*diag),
                          arcball->distToCentre + 1.2*diag );

  mat4 M, V; // will be set in the following if statements

  if (!trainView) { //determines which view to display based on whether trainView is active or not
    if (doRead) {
        arcball->setV({0, 0, 1000}, {0, 0, 0}, {0, 1, 0});
        doRead = false;
    }
      M = translate(-1 * (int)(terrain->texture->width) / 2, -1 * (int)(terrain->texture->height) / 2, 0); // default M and V transforms
      V = arcball->V;
  }
  else { // train view M and V transforms
      vec3 o, x, y, z;

      arcball->centreViewpoint();

      float t = spline->paramAtArcLength(train->getPos());
      spline->findLocalSystem(t, o, x, y, z);

      M = translate(0, 0, -3) * identity4(); // moves viewpoint up a bit so it does not go through the tops of pillars

      V.rows[0] = vec4(x, -1 * o * x); // V matrix creation
      V.rows[1] = vec4(y, -1 * o * y);
      V.rows[2] = vec4(z, -1 * o * z);
      V.rows[3] = vec4(0, 0, 0, 1);

      mat4 rotate180; // to rotate the viewing direction by 180 degrees so it faces forwards
      rotate180.rows[0] = vec4(1, 0, 0, 0);
      rotate180.rows[1] = vec4(0, 1, 0, 0);
      rotate180.rows[2] = vec4(0, 0, -1, 0);
      rotate180.rows[3] = vec4(0, 0, 0, 1);

      V = rotate180 * V;
  }

  mat4 MV = V * M;
  mat4 MVP = VCStoCCS * MV;

  vec3 lightDir = vec3( LIGHT_DIR ).normalize();

  // Draw control points

  ctrlPoints->draw( drawTrack, MV, MVP, lightDir, POST_COLOUR );

  if (ctrlPoints->count() > 1) {
      if (drawTrack)
        drawAllTrack( MV, MVP, lightDir );
    else { // Draw spline
      if (useArcLength)
        spline->drawWithArcLength( MV, MVP, lightDir, debug );
      else
        spline->draw( MV, MVP, lightDir, debug );
    }
  }

  // Draw heightfield

  gpu->activate();

  gpu->setMat4( "MV",  MV );
  gpu->setMat4( "MVP", MVP );

  gpu->setVec3( "lightDir", lightDir );

  terrain->draw( MV, MVP, lightDir, drawUndersideOnly );

  gpu->deactivate();

  // Draw train

  if (ctrlPoints->count() > 1 && drawCoaster)
    if (!trainView) // stops train from being drawin in train view so the viewpoint is not inside the cube
        train->draw( MV, MVP, lightDir, flag );

  // Now the axes

  if (showAxes) {
    MVP = VCStoCCS * V * scale(10,10,10); // no object transformation, since axes are already at origin and aligned.
    axes->draw( MVP );
  }

  // Draw Skybox (remove translation from view matrix)
  // also note arcball->VRotationOnly is not used because we also need to remove the translation from V if in train view

  V[0][3] = 0;
  V[1][3] = 0;
  V[2][3] = 0;
  V = V * rotate(90 * M_PI / 180, vec3(1, 0, 0));
  cubemap->draw(V, VCStoCCS); // (cubemap shaders take only V and P)

  // Done

  glfwSwapBuffers( window );
}



// Key callback


void World::keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )

{
  if (action == GLFW_PRESS)
    switch (key) {

    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose( window, GL_TRUE );
      break;

    case 'D':
      debug = !debug;           // enable/disable debugging
      break;

    case 'T':
      if (!trainView) // guard to stop user from disabling track drawing when in train view
        drawTrack = !drawTrack;   // enable/disable track drawing
      break;

    case 'C':
      drawCoaster = !drawCoaster; // enable/disable coaster drawing
      break;

    case 'A':
      useArcLength = !useArcLength; // enable/disable arc-length drawing
      break;

    case 'U':
      drawUndersideOnly = !drawUndersideOnly; // enable/disable drawing of terrain
      break;

    case 'P':
      pause = !pause;
      break;

    case 'F':
      flag = !flag;  // general purpose
      break;

    case 'M':                   // change the change-of-basis matrix
      spline->nextCOB();
      break;

    case '+':
    case '=':
      train->accelerate();
      break;

    case '-':
    case '_':
      train->brake();
      break;

    case 'X':
      showAxes = !showAxes;
      break;

    case 'V':
      trainView = !trainView; // toggle train view
      if (trainView) {
            doRead = true;
          if (drawTrack == false) // if entering train view and track is not toggled toggle it
              drawTrack = !drawTrack;
      }
      break;

    case '/':  // = ?
      cout << "Click to add a control point." << endl
           << "Ctrl-click to delete a control point." << endl
           << "Move a control point by dragging its base." << endl
           << "Change a control point's height by dragging its top." << endl
           << endl
	       << "-/+ change train speed" << endl
           << "a - toggle arc length parameterization" << endl
           << "c - toggle coaster drawing" << endl
           << "d - toggle debug mode (shows local coordinate frame on track)" << endl
           << "f - toggle flag (useful for debugging)" << endl
           << "m - cycle through CoB matrices" << endl
           << "p - toggle pause" << endl
           << "t - toggle track drawing" << endl
           << "u - toggle underside of terrain" << endl
           << "w - write initial view (for use on next startup)" << endl
           << "x - toggle world axes" << endl
           << "v - toggle train view" << endl // added to help message
        ;

        break;
    }
}

// Return rayStart and rayDir for the ray in the WCS from the
// viewpoint through the current mouse position (mouseX,mouseY).

void World::getMouseRay( int mouseX, int mouseY, vec3 &rayStart, vec3 &rayDir )

{
  vec4 ccsMouse( mouseX/(float)windowWidth*2-1, -1 * (mouseY/(float)windowHeight*2-1), 0, 1 );  // [-1,1]x[-1,1]x0x1
  vec3 wcsMouse = ((VCStoCCS * arcball->V).inverse() * ccsMouse).toVec3();

  rayStart = arcball->eyePosition();
  rayDir   = (wcsMouse - rayStart).normalize();
}




void World::mouseButtonCallback( GLFWwindow* window, int button, int action, int keyModifiers )

{
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

      // 'arcball' handles right mouse event

      arcballActive = true;

      arcball->mousePress( button, xpos, ypos );

    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {

      // 'World' handles left mouse events

      static vec3 lastMousePos;

      lastMousePos.x = xpos;
      lastMousePos.y = ypos;

      draggingWasDone = false;

      vec3 start,dir;
      getMouseRay( xpos, ypos, start, dir ); // sets 'start' and 'dir'

      mat4 M = translate( -1*(int)(terrain->texture->width)/2, -1*(int)(terrain->texture->height)/2, 0 );

      int hitID = ctrlPoints->findSelectedPoint( start, dir, M );

      if (hitID >= 0) {

        // mouse has picked a control point handle

        startMousePos = vec3( xpos, ypos, 0 );

        dragging = true;

        selectedCtrlPoint = hitID / 2;
        movingSelectedBase = ((hitID % 2) == 0); // base names are even; top names are odd
        startCtrlPointPos = ctrlPoints->points[selectedCtrlPoint];
      }
    }

  } else {

    // handle mouse release

    if (!draggingWasDone && button == GLFW_MOUSE_BUTTON_LEFT)
      mouseClick( vec3( xpos, ypos, 0 ), keyModifiers );

    glfwSetCursorPosCallback( window, NULL );
    dragging = false;
    arcballActive = false;
  }
}




// Mouse callbacks

void World::mouseMovementCallback( GLFWwindow* window, double xpos, double ypos )

{
  if (arcballActive) {
    arcball->mouseDrag( xpos, ypos );
    return;
  }

  if (!dragging)
    return;

  draggingWasDone = true;

  // Find mouse in world

  vec3 start,dir;
  getMouseRay( xpos, ypos, start, dir ); // sets 'start' and 'dir'

  vec3 updir = arcball->upDirection();
  vec3 n     = (dir ^ updir).normalize();

  mat4 M = translate( -1*(int)(terrain->texture->width)/2, -1*(int)(terrain->texture->height)/2, 0 );

  // Perform the action

  if (movingSelectedBase) {

    // Moving the base along the terrain

    vec3 intPoint;
    bool found = terrain->findIntPoint( start, dir, n, intPoint, M );

    if (found)
      ctrlPoints->moveBase( selectedCtrlPoint, intPoint );

  } else {

    // Moving the top up or down

    // Find the plane through eye and mouseInWorld

    vec3 n = ((dir ^ updir) ^ dir).normalize();
    float d = n * start;

    // Find where the vertical line from the base upward intersects this plane
    //
    // Solve for t in n*(base + t*vertical) = d

    vec3 base = (M * vec4( ctrlPoints->bases[ selectedCtrlPoint ], 1 )).toVec3();
    vec3 vertical(0,0,1);

    if (fabs(n*vertical) < 0.001)
      return;

    float t = (d - n*base) / (n*vertical);

    if (t < 1)
      return;

    ctrlPoints->setHeight( selectedCtrlPoint, t );
  }
}



void World::mouseScrollCallback( GLFWwindow* window, double xoffset, double yoffset )

{
  arcball->mouseScroll( xoffset, yoffset ); // tell arcball about this
}



void World::mouseClick( vec3 mousePosition, int keyModifiers )

{
  // Find ray from eye through mouse

  mat4 M = translate( -1*(int)(terrain->texture->width)/2, -1*(int)(terrain->texture->height)/2, 0 );

  vec3 start,dir;
  getMouseRay( mousePosition.x, mousePosition.y, start, dir ); // sets 'start' and 'dir'

  vec3 updir = arcball->upDirection();
  vec3 n     = (dir ^ updir).normalize();

  if (keyModifiers & GLFW_MOD_CONTROL) {

    // CTRL is held down.  Delete the control point under the mouse

    int hitID = ctrlPoints->findSelectedPoint( start, dir, M );

    if (hitID >= 0)
      ctrlPoints->deletePoint( hitID/2 ); // IDs i and i+1 are for the bottom/top of a post.  Post ID is i/2.

  } else {

    // CTRL is not held down.  Insert a new control point.

    vec3 intPoint;

    if (terrain->findIntPoint( start, dir, n, intPoint, M ))
      ctrlPoints->addPoint( intPoint );
  }
}

// Draw the track


#define DIST_BETWEEN_TIES 15.0
#define NUM_SEGMENTS_BETWEEN_TIES 4


void World::drawAllTrack( const mat4 &MV, const mat4 &MVP, vec3 lightDir )
{
    float totalLength = spline->totalArcLength();

    for (float s = 0; s < totalLength; s += totalLength / (float)(spline->data.size() * 20)) {
        float t = spline->paramAtArcLength(s + 2); // s + 2 makes linear interpolation especially connect better

        // set modelling transform for the 3 pieces of track (both rails and wood)
        mat4 M1 = spline->findLocalTransform(t) * translate(1.5, -1, 0) * scale(0.5, 0.75, totalLength / (float)(spline->data.size() * 20) + 0.5);
        mat4 M2 = spline->findLocalTransform(t) * translate(-1.5, -1, 0) * scale(0.5, 0.75, totalLength / (float)(spline->data.size() * 20) + 0.5);
        mat4 M3 = spline->findLocalTransform(t) * translate(0.0, -1.5, 0.0) * scale(4.0, 0.25, 1.0) * rotate(90 * M_PI / 180, vec3(1, 0, 0));

        // draw 3 pieces of track
        cube->draw(MV * M1, MVP * M1, lightDir, vec3(135 / 255.0, 135 / 255.0, 135 / 255.0));
        cube->draw(MV * M2, MVP * M2, lightDir, vec3(135 / 255.0, 135 / 255.0, 135 / 255.0));
        cube->draw(MV * M3, MVP * M3, lightDir, vec3(164 / 255.0, 116 / 255.0, 73 / 255.0));
    }
}
