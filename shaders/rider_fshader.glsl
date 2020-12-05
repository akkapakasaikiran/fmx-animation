#version 330

in vec3 normal;
in vec3 eye;
in vec4 COLOR;
in vec2 texCoord;

out vec4 frag_color;

uniform vec4 light_stat;
uniform vec4 headlight;
uniform mat4 ModelviewMatrix;

uniform sampler2D Texture;

void main() 
{
	// Defining Materials
	vec4 diffuse = vec4(0.5, 0.5, 0.5, 1.0); 
	vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);
	vec4 specular = vec4(1.0, 0.5, 0.5, 1.0);
	float shininess = 0.05;
	vec4 spec = vec4(0.0); 

	// Defining Light 
	vec3 lightPos = vec3(50.0, 50.0, -40.0); //in eye coordinates
	vec3 lightDir = normalize(vec3(lightPos - eye));

	//Diffuse
	vec3 n = normalize(vec3(normal));	     
	float dotProduct = dot(n, lightDir);
	float intensity =  max(dotProduct, 0.0);

	// Compute specular component only if light falls on vertex
	if(intensity > 0.0){
		vec3 v = normalize(-eye.xyz);
		vec3 r = reflect(-lightDir, normal);
		float intSpec = max(dot(r,v), 0.0); 
		spec = specular * pow(intSpec, shininess);
	}  
	
	vec4 tmp_color = COLOR;
	if(texCoord != vec2(-1.0, -1.0)){
		tmp_color = texture2D(Texture, texCoord);
	}

	vec4 color;
	color = (intensity * diffuse + spec + ambient) * tmp_color; 

	frag_color = tmp_color;
	
}
