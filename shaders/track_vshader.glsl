// #version 330

// in vec4 vPosition;
// in vec4 vColor;
// in vec4 vNormal;
// in vec2 vTexCoord;

// out vec3 normal;
// out vec3 eye;
// out vec4 COLOR;
// out vec2 texCoord;

// uniform mat4 MVP;
// uniform mat3 normalMatrix;
// uniform mat4 ModelviewMatrix;

// void main (void)
// {
// 	gl_Position = MVP * vPosition;
// 	normal = normalize(normalMatrix * vec3(vNormal)); 
// 	eye = vec3(ModelviewMatrix * vPosition); 
// 	COLOR = vColor; 
// 	texCoord = vTexCoord;
// }

#version 330

in vec4 vPosition;
in vec4 vColor;
in vec4 vNormal;

out vec3 normal;
out vec3 eye;
out vec4 COLOR;

uniform mat4 MVP;
uniform mat3 normalMatrix;
uniform mat4 ModelviewMatrix;
void main (void) 
{
  gl_Position = MVP * vPosition;
  normal = normalize(normalMatrix * vec3(vNormal)); 
  eye = vec3(ModelviewMatrix * vPosition); 
  COLOR = vColor; 
}