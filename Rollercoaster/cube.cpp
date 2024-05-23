// cube.cpp

#include "cube.h"


void Cube::setupVAO()

{
    // Set up buffers of vertices and normals.  These are
    // collected from the cube's 'seq<vec3> verts' and
    // 'seq<vec3> normals'.

    // copy vertices

    int nVerts = verts.size();

    GLfloat* vertexBuffer = new GLfloat[nVerts * 3];

    for (int i = 0; i < nVerts; i++)
        ((vec3*)vertexBuffer)[i] = verts[i];

    // copy normals

    GLfloat* normalBuffer = new GLfloat[nVerts * 3];

    for (int i = 0; i < nVerts; i++)
        ((vec3*)normalBuffer)[i] = normals[i];

    // Create a VAO

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // store vertices (i.e. one triple of floats per vertex)

    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

    glBufferData(GL_ARRAY_BUFFER, nVerts * 3 * sizeof(GLfloat), vertexBuffer, GL_STATIC_DRAW);

    // attribute 0 = position

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // store vertex normals (i.e. one triple of floats per vertex)

    GLuint normalBufferID;
    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);

    glBufferData(GL_ARRAY_BUFFER, nVerts * 3 * sizeof(GLfloat), normalBuffer, GL_STATIC_DRAW);

    // attribute 1 = normal

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Clean up

    delete[] vertexBuffer;
    delete[] normalBuffer;
}


void Cube::draw(const mat4& MV, const mat4& MVP, vec3 lightDir, vec3 colour)

{
    gpu.activate();

    gpu.setMat4("MV", MV);
    gpu.setMat4("MVP", MVP);
    gpu.setVec3("colour", colour);
    gpu.setVec3("lightDir", lightDir);

    // Draw using element array

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 * 6); // used GL_TRIANGLE_STRIP so that only 4 verticies needed to be defined per face instead of 6 (GL_TRIANGLE_STRIP draws triangles using v0, v1, v2 and v2, v1, v3)
    glBindVertexArray(0);

    gpu.deactivate();
}

const char* Cube::vertShader = R"(

  #version 330 es

  uniform mat4 MVP;
  uniform mat4 MV;

  layout (location = 0) in mediump vec3 vertPosition;
  layout (location = 1) in mediump vec3 vertNormal;

  smooth out mediump vec3 normal;

  void main() {

    gl_Position = MVP * vec4( vertPosition, 1.0 );
    normal = vec3( MV * vec4( vertNormal, 0.0 ) );
  }
)";


const char* Cube::fragShader = R"(

  #version 330 es

  uniform mediump vec3 colour;
  uniform mediump vec3 lightDir;

  smooth in mediump vec3 normal;
  out mediump vec4 outputColour;

  void main() {

    mediump float NdotL = dot( normalize(normal), lightDir );

    if (NdotL < 0.0)
      NdotL = 0.1; // some ambient

    outputColour = vec4( NdotL * colour, 1.0 );
  }
)";





