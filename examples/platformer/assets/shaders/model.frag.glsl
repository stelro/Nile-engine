#version 420 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

struct Material {
  sampler2D ambient1;
  sampler2D diffuse1;
  sampler2D specular1;
  float shininess;
};

struct DirectionalLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;

  // used for the attenuation
  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

#define NR_OF_POINT_LIGHTS 4

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NR_OF_POINT_LIGHTS];

uniform Material material;

vec4 calcDirLight( DirectionalLight light, vec3 normal, vec3 viewDir );
vec4 calcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir );

void main() {

  vec3 normal = normalize( Normal );
  vec3 viewDirection = normalize( viewPos - FragPos );

  vec4 result = calcDirLight( directionalLight, normal, viewDirection );

  for ( int i = 0; i < NR_OF_POINT_LIGHTS; i++) {
    result += calcPointLight( pointLights[i], normal, FragPos, viewDirection );
  }

  FragColor = vec4( result );
}

vec4 calcDirLight( DirectionalLight light, vec3 normal, vec3 viewDir ) {

  // We negate the light.direction, because we want specifie the light to point */
  // from the light source towards the fragment */
  vec3 lightDirection = normalize( -light.direction );

  float diff = max( dot( normal, lightDirection ), 0.0 );
  vec3 reflectDirecion = reflect( -lightDirection, normal );
  float spec = pow( max( dot( viewDir, reflectDirecion ), 0.0 ), material.shininess );


  vec4 ambient = vec4( light.ambient, 1.0 ) * texture( material.ambient1, TexCoords );
  vec4 diffuse = vec4( light.diffuse, 1.0 ) * diff * texture( material.diffuse1, TexCoords );


  vec4 specular = vec4( light.specular, 1.0 ) * spec * texture( material.specular1, TexCoords );


  return ( ambient + diffuse + specular );
}

vec4 calcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir ) {


  vec3 lightDir = normalize( light.position - fragPos );

  float diff = max( dot( normal, lightDir ), 0.0 );
  vec3 reflectDirecion = reflect( -lightDir, normal );
  float spec = pow( max( dot( viewDir, reflectDirecion ), 0.0 ), material.shininess );

  // attenuation
  float distance = length( light.position - fragPos );
  float attenuation = 1.0 / ( light.constant + light.linear * distance +
                              light.quadratic * ( distance * distance ) );

  vec4 ambient = vec4( light.ambient, 1.0 ) * texture( material.ambient1, TexCoords );
  vec4 diffuse = vec4( light.diffuse, 1.0 ) * diff * texture( material.diffuse1, TexCoords );


  vec4 specular = vec4( light.specular, 1.0 ) * spec * texture( material.specular1, TexCoords );

  diffuse *= attenuation;
  specular *= attenuation;

  return ( ambient + diffuse + specular );
  //return ( ambient + diffuse + specular ) * vec4(3.4, 0.2, 1.0, 1.0);
}
