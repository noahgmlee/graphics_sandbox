// terrain.h

#ifndef TERRAIN_H
#define TERRAIN_H

#include "headers.h"
#include "texture.h"
#include "seq.h"
#include "gpuProgram.h"

/*
the rollercoaster terrain consists of 2 textures: height and colour
the height texture is a greyscale value at each 2D x, y point indicating hill height
the colour texture is simply the colour at each 2D x, y point
*/
class Terrain {

  //colour value at each point. point height determined by height texture
  vec3 **points;

  //normal to each point for lighting
  vec3 **normals;

  seq<vec3> quadsToHighlight;

  bool rayTriangleInt( vec3 rayStart, vec3 rayDir, vec3 v0, vec3 v1, vec3 v2, vec3 & intPoint, float & intParam );

  GLuint      VAO;
  GPUProgram  gpu;
  int         nFaces;

  static const char *vertShader;
  static const char *fragShader;

 public:

  Texture *heightfield;
  Texture *texture;

  Terrain( string basePath, string heightfieldFilename, string textureFilename ) {
    readTextures( basePath, heightfieldFilename, textureFilename );
    gpu.init( vertShader, fragShader, "in terrain.cpp" );
    setupVAO();
  }

  void readTextures( string basePath, string heightfieldFilename, string textureFilename );
  void setupVAO();
  void draw( mat4 &MV, mat4 &MVP, vec3 lightDir, bool drawUndersideOnly );

  bool findIntPoint( vec3 rayStart, vec3 rayDir, vec3 planePerp, vec3 &intPoint, mat4 &M );
};

#endif
