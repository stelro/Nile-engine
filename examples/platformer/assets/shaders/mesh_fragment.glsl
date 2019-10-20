#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform sampler2D image;
uniform vec3 spriteColor;

void main() {
  color = texture(image, TexCoords) * vec4(spriteColor, 1.0f);
}
 
