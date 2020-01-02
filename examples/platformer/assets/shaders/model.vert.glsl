#version 420 core
layout( location = 0 ) in vec3 aPos;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  TexCoords = aTexCoords;
  // @fix: this is costly operation
  // it's a better to calculate the normal matrix in the CPU
  // and send it to the shaders via uniform object
  Normal = mat3( transpose( inverse( model ) ) ) * aNormal;
  FragPos = vec3( model * vec4( aPos, 1.0 ) );
  gl_Position = projection * view * model * vec4( aPos, 1.0 );
}
