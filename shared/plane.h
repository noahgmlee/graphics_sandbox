// plane.h
//
// Set up and draw a plane


#ifndef PLANE_H
#define PLANE_H


#include "linalg.h"
#include "gpuProgram.h"


class Plane {

  GLuint VAO;
  GPUProgram program;
  static const char *vertShader;
  static const char *fragShader;

 public:

  Plane(vec3 b1, vec3 b2);
  void draw( mat4 &MVP_transform );
};


#endif
