// vertex shader

#version 330 es

uniform mat4 M;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vcolour;

out mediump vec3 colour;

void main()

{
  gl_Position = M * vec4( position, 1 );
  colour = vcolour ;
}
