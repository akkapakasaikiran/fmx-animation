#version 330

in vec3 normal;
in vec3 eye;
in vec4 COLOR;
in vec2 texCoord;

out vec4 frag_color;

uniform vec4 light_stat;
uniform vec4 headlight;
uniform vec4 spotlight;
uniform mat4 ModelviewMatrix;
uniform vec3 nrml;
uniform mat4 ViewMatrix;

uniform sampler2D Texture;

void main() 
{
	// Defining Materials
	vec4 diffuse = vec4(0.4, 0.4, 0.4, 1.0); 
	vec4 ambient = vec4(0.01, 0.01, 0.01, 1.0);
	vec4 specular = vec4(0.4, 0.4, 0.4, 1.0);
	float shininess = 1;
	vec4 spec = vec4(0.0); 

	// Defining Light 1
	vec4 lightPos1 = vec4(0.0, 60.0, 40.0, 1.0); //in world coordinates
	vec3 lightPos = vec3(ViewMatrix * lightPos1); //in view coordinates
	vec3 lightDir = normalize(vec3(lightPos - eye));

	//Diffuse
	vec3 n = normalize(vec3(normal));	     
	float dotProduct = dot(n, lightDir);
	float intensity =  max( dotProduct, 0.0);

	// Compute specular component only if light falls on vertex
	if(intensity > 0.0){
		vec3 v = normalize(-eye.xyz);
		vec3 r = reflect(-lightDir, normal);
		float intSpec = max(dot(r,v), 0.0); 
		spec = specular * pow(intSpec, shininess);
	}  
	
	vec4 tmp_color = COLOR;
	if(texCoord != vec2(-1.0, -1.0)){
		tmp_color = texture(Texture, texCoord);
	}

	vec4 color;
	if(light_stat[0] == 1.0) color = (intensity * diffuse + spec) * tmp_color + ambient; 
	else color = ambient;



	// Defining Head Light 
	lightPos = vec3(headlight); //in eye coordinates
	lightDir = normalize(vec3(lightPos - eye));

	// Diffuse
	n = normalize(vec3(normal));      
	dotProduct = dot(n, lightDir);
	intensity =  max( dotProduct, 0.0);

	float dotProduct1 = dot(normalize(nrml), -lightDir);

	// Compute specular component only if light falls on vertex
	if(intensity > 0.0){
		vec3 v = normalize(-eye.xyz);
		vec3 r = reflect(-lightDir, normal);
		float intSpec = max(dot(r,v), 0.0); 
		spec = specular * pow(intSpec, shininess);
	}  

	if(light_stat[2] == 1.0){
		//if(dotProduct1 > 0.8)
      		color += (intensity * diffuse + spec) * tmp_color + ambient;
    	//else
      		//color += ambient;
	}
	else color += ambient;


	// Defining Light 2
  	// vec3 lightPos = vec3(0.0, 0.0, 0.0); //in eye coordinates
  	lightPos1 = vec4(0.0, 60.0, -40.0, 1.0); //in world coordinates
	lightPos = vec3(ViewMatrix * lightPos1); //in view coordinates
  	lightDir = normalize(vec3(lightPos - eye));

  	//Diffuse
  	n = normalize(vec3(normal));      
  	dotProduct = dot(n, lightDir);
  	intensity =  max( dotProduct, 0.0);

  	// Compute specular component only if light falls on vertex
  	if(intensity > 0.0)
  	{
    	vec3 v = normalize(-eye.xyz);
    	vec3 r = reflect(-lightDir, normal);
    	float intSpec = max(dot(r,v), 0.0); 
        spec = specular * pow(intSpec, shininess);
  	}  

  	if(light_stat[1] == 1.0){
    	color += (intensity * diffuse + spec)*tmp_color + ambient; // All
  	}
  	else{
    	color += ambient;
  	}


  	// Defining Spot Light 
	lightPos = vec3(spotlight); //in eye coordinates
	lightDir = normalize(vec3(lightPos - eye));

	// Diffuse
	n = normalize(vec3(normal));      
	dotProduct = dot(n, lightDir);
	intensity =  max( dotProduct, 0.0);

	// Compute specular component only if light falls on vertex
	if(intensity > 0.0){
		vec3 v = normalize(-eye.xyz);
		vec3 r = reflect(-lightDir, normal);
		float intSpec = max(dot(r,v), 0.0); 
		spec = specular * pow(intSpec, shininess);
	}  

	if(light_stat[3] == 1.0) color += (intensity * diffuse + spec) * tmp_color + ambient;
	else color += ambient;

	frag_color = color;
	
}
