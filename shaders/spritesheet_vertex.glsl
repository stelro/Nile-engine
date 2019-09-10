#version 330 core

layout( location = 0 ) in vec4 vertex;

uniform mat4 model;
uniform mat4 projection;
uniform vec2 st;
// TODO(stel): actually this is the rows/cols of the sprite sheet
// I should find a better name for this
// x  -> rows, y -> cols
uniform vec2 size;

out vec2 TexCoords;

void main() {
  TexCoords = vec2(vertex.z / size.y + st.s, vertex.w / size.x + st.t);
  gl_Position = projection * model * vec4( vertex.xy, 0.0, 1.0 );
}
