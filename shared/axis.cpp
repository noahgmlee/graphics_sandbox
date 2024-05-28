// axis.cpp


#include "headers.h"
#include "axis.h"


Axis::Axis(vec3 verts)

{
  // VAO

  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  // Positions

  vec3 vertices[] = {
    vec3(0,0,0), verts
  };

  GLuint vertexbuffer;
  glGenBuffers( 1, &vertexbuffer );
  glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
  glBufferData( GL_ARRAY_BUFFER, 2 * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW );
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  // Colours

  vec3 colours[] = {
    vec3(1.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0),
  };

  GLuint colourbuffer;
  glGenBuffers( 1, &colourbuffer );
  glBindBuffer( GL_ARRAY_BUFFER, colourbuffer );
  glBufferData( GL_ARRAY_BUFFER, 2 * 3 * sizeof(GLfloat), colours, GL_STATIC_DRAW );
  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  // shaders

  program.init( vertShader, fragShader, "in axis.cpp" );
}


const char *Axis::vertShader = R"(

  #version 330 es

  uniform mat4 MVP;

  layout (location = 0) in mediump vec3 vertPosition;
  layout (location = 1) in mediump vec3 vertColour;

  smooth out mediump vec3 colour;

  void main() {
    gl_Position = MVP * vec4( vertPosition, 1.0f );
    colour = vertColour;
  }
)";


const char *Axis::fragShader = R"(

  #version 330 es

  smooth in mediump vec3 colour;
  out mediump vec4 outputColour;

  void main() {
    outputColour = vec4( colour, 1.0 );
  }
)";



void Axis::draw( mat4 &MVP_transform )

{
  program.activate();

  program.setMat4( "MVP", MVP_transform );

#ifndef __APPLE__
  glLineWidth( 3.0 );
#endif

  glBindVertexArray( VAO );
  glDrawArrays( GL_LINES, 0, 2 );

#ifndef __APPLE__
  glLineWidth( 1.0 );
#endif

  program.deactivate();
}

