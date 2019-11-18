#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() {

  vec3 col = texture( screenTexture, TexCoords ).rgb;
  FragColor = vec4( vec3( 1.0f - texture( screenTexture, TexCoords ) ), 1.0f );
}
