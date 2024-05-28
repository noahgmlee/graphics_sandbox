#include "frustum.h"

static constexpr float MOVE_RATE = 0.01;
static constexpr float ROT_RATE = 0.005; //radians


mat4 Frustum::getV() {
    vec3 x = vec3(M.rows[0].x, M.rows[1].x, M.rows[2].x);
    vec3 y = vec3(M.rows[0].y, M.rows[1].y, M.rows[2].y);
    vec3 z = vec3(M.rows[0].z, M.rows[1].z, M.rows[2].z);
    vec3 t = vec3(M.rows[0].w, M.rows[1].w, M.rows[2].w); //translative model to world
    mat4 V;
    V.rows[0] = vec4(x.normalize(), -1 * t * x.normalize());
    V.rows[1] = vec4(y.normalize(), -1 * t * y.normalize());
    V.rows[2] = vec4(z.normalize(), -1 * t * z.normalize());
    V.rows[3] = vec4(0, 0, 0, 1);
    return V;
}

mat4 Frustum::getP() {
    return frustum(8.0/6.0 * -0.05, 8.0/6.0 * 0.05, -0.05, 0.05, 0.1, 1.0);
}

void Frustum::horizontalMove(bool dir) {
    if (dir) {
        T = translate({MOVE_RATE * M.rows[0].x,
                       MOVE_RATE * M.rows[1].x,
                       MOVE_RATE * M.rows[2].x,});
        M = T * M;
    } else {
        T = translate({-MOVE_RATE * M.rows[0].x,
                       -MOVE_RATE * M.rows[1].x,
                       -MOVE_RATE * M.rows[2].x,});
        M = T * M;
    }
}

void Frustum::verticalMove(bool dir) {
    if (dir) {
        T = translate({MOVE_RATE * M.rows[0].y,
                       MOVE_RATE * M.rows[1].y,
                       MOVE_RATE * M.rows[2].y,});
        M = T * M;
    } else {
        T = translate({-MOVE_RATE * M.rows[0].y,
                       -MOVE_RATE * M.rows[1].y,
                       -MOVE_RATE * M.rows[2].y,});
        M = T * M;
    }
}

void Frustum::horizontalRot(bool dir) {
    vec4 translated = M * vec4(0,0,0,1);
    Tinverse = translate(vec3(0 - translated.x, 0 - translated.y, 0 - translated.z));
    T = translate(vec3(translated.x, translated.y, translated.z));
    vec3 XYplane = {M.rows[0].y, M.rows[1].y, M.rows[2].y};
    if (dir) {
        thetaX += ROT_RATE;
        R = rotate(-thetaX, XYplane);
        M = T * R * Tinverse * M;
    } else {
        thetaX += ROT_RATE;
        R = rotate(thetaX, XYplane);
        M = T * R * Tinverse * M;
    }
}

void Frustum::verticalRot(bool dir) {
    vec4 translated = M * vec4(0,0,0,1);
    Tinverse = translate(vec3(0 - translated.x, 0 - translated.y, 0 - translated.z));
    T = translate(vec3(translated.x, translated.y, translated.z));
    vec3 YZplane = {M.rows[0].x, M.rows[1].x, M.rows[2].x};
    if (dir) {
        thetaY += ROT_RATE;
        R = rotate(thetaY, YZplane);
        M = T * R * Tinverse * M;
    } else {
        thetaY += ROT_RATE;
        R = rotate(-thetaY, YZplane);
        M = T * R * Tinverse * M;
    }
}

void Frustum::expandView(bool bigger) {
    if (bigger) {
        S = scale(1.1, 1.1, 1.0);
        nearY *= 1.1;
        M = M * S;
    } else {
        S = scale(1.0/1.1, 1.0/1.1, 1.0);
        nearY *= 1.0/1.1;
        M = M * S;
    }
    fovy = atan2(2*nearY, 0.1); //2*y because its the angle around the eye
}

// Convert mouse position to [-1,1]x[-1,1]
vec2 Frustum::normalizeMousePos( float x, float y ) {
    int width, height;
    glfwGetWindowSize( window, &width, &height );
    float w = width/2.0;
    float h = height/2.0;
    return vec2( (x - w)/w, (h - y)/h ); // flip in y direction
}

void Frustum::draw( mat4& MV, mat4& MVP, mat4& axesMVP) {
    gpu.activate();

    gpu.setMat4("MV", MV);
    gpu.setMat4("MVP", MVP);

    // Draw using element array

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, FRUSTUM_VERTICES); // used GL_TRIANGLE_STRIP so that only 4 verticies needed to be defined per face instead of 6 (GL_TRIANGLE_STRIP draws triangles using v0, v1, v2 and v2, v1, v3)
    glBindVertexArray(0);

    gpu.deactivate();

    axes->draw(axesMVP);
}

void Frustum::setupVAO() {
    // Set up buffers of vertices and normals.  These are
    // collected from the cube's 'seq<vec3> verts' and
    // 'seq<vec3> normals'.

    // copy vertices
    GLfloat* vertexBuffer = new GLfloat[FRUSTUM_VERTICES * 3];

    for (int i = 0; i < FRUSTUM_VERTICES; i++)
        ((vec3*)vertexBuffer)[i] = verts[i];

    // Create a VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // store vertices (i.e. one triple of floats per vertex)

    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

    glBufferData(GL_ARRAY_BUFFER, FRUSTUM_VERTICES * 3 * sizeof(GLfloat), vertexBuffer, GL_STATIC_DRAW);

    // attribute 0 = position

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Clean up

    delete[] vertexBuffer;
}

const char* Frustum::vertShader = R"(

  #version 330 es

  uniform mat4 MVP;
  uniform mat4 MV;

  layout (location = 0) in mediump vec3 vertPosition;

  void main() {
    gl_Position = MVP * vec4( vertPosition, 1.0 );
  }
)";


const char* Frustum::fragShader = R"(

  #version 330 es

  out mediump vec4 outputColour;

  void main() {
    outputColour = vec4( 1.0, 1.0, 1.0, 1.0);
  }
)";
