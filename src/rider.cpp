#include "common.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_TGA
#include "stb_image_read.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.hpp"

GLuint shaderProgram;
GLuint skyshaderProgram;

glm::mat4 rotation_matrix;
glm::mat4 projection_matrix;
glm::mat4 c_rotation_matrix;
glm::mat4 lookat_matrix;
glm::mat4 model_matrix;
glm::mat4 view_matrix;
glm::mat4 modelview_matrix;
glm::mat3 normal_matrix;

GLuint MVP;
GLuint ModelviewMatrix;
GLuint normalMatrix;

// skybox
GLuint view;
GLuint projection;
GLuint cubemapTexture;
GLuint skyboxVBO, skyboxVAO;


const int num_vertices = 36;

//////////////////////////////////////////////////////////////////////////////////////////

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

// Write into positions with an array of eight vertices in homogenous coordinates
void cube_coords(glm::vec4* positions){
	positions[0] = glm::vec4(-1, -1, 1, 1.0);
	positions[1] = glm::vec4(-1, 1, 1, 1.0);
	positions[2] = glm::vec4(1, 1, 1, 1.0);
	positions[3] = glm::vec4(1, -1, 1, 1.0);
	positions[4] = glm::vec4(-1, -1, -1, 1.0);
	positions[5] = glm::vec4(-1, 1, -1, 1.0);
	positions[6] = glm::vec4(1, 1, -1, 1.0);
	positions[7] = glm::vec4(1, -1, -1, 1.0);
}

// Makes a square face using positions 
void quad(int a, int b, int c, int d, int i, glm::vec4* normals_arr, 
	glm::vec4* posns_arr, glm::vec2* texcoord_arr, glm::vec4* positions)
{
	glm::vec4 posba = positions[b] - positions[a];
	glm::vec4 posca = positions[c] - positions[a];
	glm::vec3 v1 = glm::vec3(posba.x, posba.y, posba.z);
	glm::vec3 v2 = glm::vec3(posca.x, posca.y, posca.z);
	glm::vec4 normal = glm::vec4(glm::cross(v1, v2), 1.0);

	posns_arr[i] = positions[a]; normals_arr[i] = normal; i++;
	posns_arr[i] = positions[b]; normals_arr[i] = normal; i++;
	posns_arr[i] = positions[c]; normals_arr[i] = normal; i++;
	posns_arr[i] = positions[a]; normals_arr[i] = normal; i++;
	posns_arr[i] = positions[c]; normals_arr[i] = normal; i++;
	posns_arr[i] = positions[d]; normals_arr[i] = normal; i++;

	i -= 6;
	texcoord_arr[i] = glm::vec2(0.0, 1.0); i++;
	texcoord_arr[i] = glm::vec2(0.0, 0.0); i++;
	texcoord_arr[i] = glm::vec2(1.0, 0.0); i++;
	texcoord_arr[i] = glm::vec2(0.0, 1.0); i++;
	texcoord_arr[i] = glm::vec2(1.0, 0.0); i++;
	texcoord_arr[i] = glm::vec2(1.0, 1.0); i++;
}

void initcube(glm::vec4* normals_arr, glm::vec4* posns_arr, 
		glm::vec2* texcoord_arr, glm::vec4* positions, int sz) 			// sz is  36
{ 
	int i = 0;
	// Making six faces of the cube
	quad(1, 0, 3, 2, i, normals_arr, posns_arr, texcoord_arr, positions); i += sz/6;
	quad(2, 3, 7, 6, i, normals_arr, posns_arr, texcoord_arr, positions); i += sz/6;
	quad(3, 0, 4, 7, i, normals_arr, posns_arr, texcoord_arr, positions); i += sz/6;
	quad(6, 5, 1, 2, i, normals_arr, posns_arr, texcoord_arr, positions); i += sz/6;
	quad(4, 5, 6, 7, i, normals_arr, posns_arr, texcoord_arr, positions); i += sz/6;
	quad(5, 4, 0, 1, i, normals_arr, posns_arr, texcoord_arr, positions); i += sz/6;
}

//////////////////////////////////////////////////////////////////////////////////////////

// Load a BMP image from filepath and write into data
unsigned char* loadImage(const char* filepath, int &w, int &h){
	unsigned char header[54];	// 54 Byte header of BMP
	int pos;
	uint size; 	// size = w * h * 3

	FILE* file = fopen(filepath, "rb"); 
	if(file == NULL)
		std::cout << "File empty\n";
	if(fread(header, 1, 54, file) != 54)
		std::cout << "Incorrect BMP file\n";

	// Read MetaData
	pos = *((int*) &(header[0x0A]));
	size = *((int*) &(header[0x22]));
	w = *((int*) &(header[0x12]));
	h = *((int*) &(header[0x16]));

	//Just in case metadata is missing
	if(size == 0) size = w * h * 3;
	if(pos == 0) pos = 54;

	unsigned char* data = new unsigned char[size];

	fread(data, size, 1, file);	 	// Read the file
	fclose(file);

	return data;
}


uint loadCubemap(std::string* faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int w, h;
	for (unsigned int i = 0; i < 6; i++){
		unsigned char *data = loadImage(faces[i].c_str(), w, h);
		if(data){
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
						 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
		free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


void initBuffersGL()
{
	// Load scene shaders 
	std::vector<GLuint> shaderList;
	shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, "shaders/rider_vshader.glsl"));
	shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, "shaders/rider_fshader.glsl"));
	shaderProgram = csX75::CreateProgramGL(shaderList);

	// Load skybox shaders 
	std::vector<GLuint> skyshaderList;
	skyshaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, "shaders/sky_vshader.glsl"));
	skyshaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, "shaders/sky_fshader.glsl"));
	skyshaderProgram = csX75::CreateProgramGL(skyshaderList);

	// Get the attributes from the shader program
	vPosition = glGetAttribLocation(shaderProgram, "vPosition");
	vColor = glGetAttribLocation(shaderProgram, "vColor"); 
	vNormal = glGetAttribLocation(shaderProgram, "vNormal");
	vTexCoord = glGetAttribLocation(shaderProgram, "vTexCoord");
	MVP = glGetUniformLocation(shaderProgram, "MVP");
	normalMatrix =  glGetUniformLocation(shaderProgram, "normalMatrix");
	ModelviewMatrix = glGetUniformLocation(shaderProgram, "ModelviewMatrix");

	// Get the attributes from the shader program
	vPos = glGetAttribLocation(skyshaderProgram, "vPos");
	view = glGetUniformLocation(skyshaderProgram, "view");
	projection = glGetUniformLocation(skyshaderProgram, "projection");

	std::string faces[6] = {
		"skybox/skybox1/px.bmp",
		"skybox/skybox1/nx.bmp",
		"skybox/skybox1/ny.bmp",
		"skybox/skybox1/py.bmp",
		"skybox/skybox1/pz.bmp",
		"skybox/skybox1/nz.bmp"
	};
	cubemapTexture = loadCubemap(faces); 

	// skybox VAO and VBO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	glm::vec4 positions[8];
	cube_coords(positions);

	glm::vec4 posns_arr[36];
	glm::vec4 normals_arr[36];
	glm::vec2 texcoord_arr[36];
	initcube(normals_arr, posns_arr, texcoord_arr, positions, 36);

	double rscale = 1;
	glm::vec4 red(1,0,0,0), sepia(0.8,0.5,0.3,0), blue(0.1,0.9,0.1,0);

	nodes["hip"] = new csX75::HNode(nodes["root"], 36, posns_arr, sizeof(posns_arr), red, normals_arr);
	nodes["hip"]->change_parameters(-1.5,0.25,1.5, 0,0,0, 0,0,0, 0,0,0);

	nodes["torso"] = new csX75::HNode(nodes["hip"], 36, posns_arr, sizeof(posns_arr), red, normals_arr, "textures/goldhill1.bmp", texcoord_arr);
	nodes["torso"]->change_parameters(0,0,0, 0,0,0, rscale*1.5,rscale*2,rscale*0.5, 0,rscale*2,0);

	nodes["neck"] = new csX75::HNode(nodes["torso"], 36, posns_arr, sizeof(posns_arr), sepia, normals_arr);
	nodes["neck"]->change_parameters(0,rscale*2.5,0, 0,0,0, rscale*0.5,rscale*0.5,rscale*0.5, 0,0,0);
	
	nodes["head"] = new csX75::HNode(nodes["neck"], 36, posns_arr, sizeof(posns_arr), sepia, normals_arr);
	nodes["head"]->change_parameters(0,0,0, 0,0,0, rscale*1,rscale*1,rscale*0.5, 0,rscale*1.5,0);

	nodes["lupper_arm"] = new csX75::HNode(nodes["torso"], 36, posns_arr, sizeof(posns_arr), red, normals_arr);
	nodes["lupper_arm"]->change_parameters(rscale*-1.5,rscale*1.5,0, 0,0,0, rscale*1,rscale*0.5,rscale*0.5, rscale*-1,0,0);

	nodes["llower_arm"] = new csX75::HNode(nodes["lupper_arm"], 36, posns_arr, sizeof(posns_arr), sepia, normals_arr);
	nodes["llower_arm"]->change_parameters(rscale*-1,0,0, 0,0,0, rscale*1,rscale*0.5,rscale*0.5, rscale*-1,0,0);

	nodes["rupper_arm"] = new csX75::HNode(nodes["torso"], 36, posns_arr, sizeof(posns_arr), red, normals_arr);
	nodes["rupper_arm"]->change_parameters(rscale*1.5,rscale*1.5,0, 0,0,0, rscale*1,rscale*0.5,rscale*0.5, rscale*1,0,0);

	nodes["rlower_arm"] = new csX75::HNode(nodes["rupper_arm"], 36, posns_arr, sizeof(posns_arr), sepia, normals_arr);
	nodes["rlower_arm"]->change_parameters(rscale*1,0,0, 0,0,0, rscale*1,rscale*0.5,rscale*0.5, rscale*1,0,0);

	nodes["lupper_leg"] = new csX75::HNode(nodes["hip"], 36, posns_arr, sizeof(posns_arr), glm::vec4(0,0,1,0), normals_arr);
	nodes["lupper_leg"]->change_parameters(rscale*-1,0,0, 0,0,0, rscale*0.5,rscale*1,rscale*0.5, 0,rscale*-1,0);

	nodes["llower_leg"] = new csX75::HNode(nodes["lupper_leg"], 36, posns_arr, sizeof(posns_arr), glm::vec4(0,0,1,0), normals_arr);
	nodes["llower_leg"]->change_parameters(0,rscale*-1,0, 0,0,0, rscale*0.5,rscale*1,rscale*0.5, 0,rscale*-1,0);
	
	nodes["rupper_leg"] = new csX75::HNode(nodes["hip"], 36, posns_arr, sizeof(posns_arr), glm::vec4(0,0,1,0), normals_arr);
	nodes["rupper_leg"]->change_parameters(rscale*1,0,0, 0,0,0, rscale*0.5,rscale*1,rscale*0.5, 0,rscale*-1,0);

	nodes["rlower_leg"] = new csX75::HNode(nodes["rupper_leg"], 36, posns_arr, sizeof(posns_arr), glm::vec4(0,0,1,0), normals_arr);
	nodes["rlower_leg"]->change_parameters(0,rscale*-1,0, 0,0,0, rscale*0.5,rscale*1,rscale*0.5, 0,rscale*-1,0);
	root_node = curr_node = nodes["hip"];
}

void renderGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	matrixStack.clear();
	matrixStack1.clear();

	// create the lookat and the up vectors for the camera
	c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f,0.0f,0.0f));
	c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f,1.0f,0.0f));
	c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f,0.0f,1.0f));

	glm::vec4 c_pos = glm::vec4(c_xpos,c_ypos,c_zpos, 1.0)*c_rotation_matrix;
	glm::vec4 c_up = glm::vec4(c_up_x,c_up_y,c_up_z, 1.0)*c_rotation_matrix;
	// create the lookat matrix
	lookat_matrix = glm::lookAt(glm::vec3(c_pos), glm::vec3(0.0), glm::vec3(c_up));

	// create the projection matrix
	projection_matrix = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 500.0);

	view_matrix = projection_matrix * lookat_matrix;

	matrixStack.push_back(view_matrix);
	matrixStack1.push_back(lookat_matrix);

	// draw scene
	glUseProgram(shaderProgram);
	root_node->render_tree();

	// draw skybox 
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glUseProgram(skyshaderProgram);

	glm::mat4 viewm = glm::mat4(glm::mat3(lookat_matrix)); // remove translation from the view matrix

	glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(viewm));
	glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS); // set depth function back to default
}

int main(int argc, char** argv)
{
	//! The pointer to the GLFW window
	GLFWwindow* window;

	//! Setting up the GLFW Error callback
	glfwSetErrorCallback(csX75::error_callback);

	//! Initialize GLFW
	if (!glfwInit())
		return -1;

	//We want OpenGL 4.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//This is for MacOSX - can be omitted otherwise
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
	//We don't want the old OpenGL 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

	//! Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(1000, 1000, "FMX Modeling", NULL, NULL);
	if (!window){
		glfwTerminate();
		return -1;
	}
	
	//! Make the window's context current 
	glfwMakeContextCurrent(window);

	//Initialize GLEW
	//Turn this on to get Shader based OpenGL
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err){
		//Problem: glewInit failed, something is seriously wrong.
		std::cerr<<"GLEW Init Failed: %s"<<std::endl;
	}

	//Print and see what context got enabled
	std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
	std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
	std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
	std::cout<<"GLSL Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;

	//Keyboard Callback
	glfwSetKeyCallback(window, csX75::key_callback);
	//Framebuffer resize callback
	glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//Initialize GL state
	csX75::initGL();
	initBuffersGL();

	// Loop until the user closes the window
	while (glfwWindowShouldClose(window) == 0)
	{
		// Render here
		renderGL();

		// Swap front and back buffers
		glfwSwapBuffers(window);
		
		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
