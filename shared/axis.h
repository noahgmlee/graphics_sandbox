// axis.h
//
// Set up and draw an axis


#ifndef AXIS_H
#define AXIS_H


#include "linalg.h"
#include "gpuProgram.h"


class Axis {

  GLuint VAO;
  GPUProgram program;
  static const char *vertShader;
  static const char *fragShader;

 public:

  Axis(vec3 verts);
  void draw( mat4 &MVP_transform );
};


#endif
