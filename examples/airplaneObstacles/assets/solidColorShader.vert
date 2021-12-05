#version 410

layout(location = 0) in vec3 inPosition;

uniform vec4 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;


out vec4 fragColor;

void main() {
  fragColor =  color;
  vec4 posEyeSpace = viewMatrix * modelMatrix * vec4(inPosition, 1);
  gl_Position = projMatrix * posEyeSpace;
}