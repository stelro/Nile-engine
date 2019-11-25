#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

uniform vec3 color;

void main() {
  vec4 texColor = texture( texture_diffuse1, TexCoords );
  FragColor = texColor;
}
