#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 normal;
in vec3 fragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

struct Material {
  sampler2D ambient1;
  sampler2D diffuse1;
  sampler2D specular1;
  float shininess;
};

struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform Material material;
uniform Light light;

void main() {

  vec4 ambient = vec4( light.ambient, 1.0 ) * texture( material.ambient1, TexCoords );

  vec3 norm = normalize( normal );
  vec3 lightDir = normalize( light.position - fragPos );

  float diff = max( dot( norm, lightDir ), 0.0 );
  vec4 diffuse = vec4( light.diffuse, 1.0 ) * diff * texture( material.diffuse1, TexCoords );

  vec3 viewDir = normalize( viewPos - fragPos );
  vec3 reflectDir = reflect( -lightDir, norm );

  float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), 32 );
  vec4 specular = vec4( light.specular, 1.0 ) * spec * texture( material.specular1, TexCoords );

  FragColor = ( ambient + diffuse + specular ) * vec4( objectColor, 1.0 );
}
