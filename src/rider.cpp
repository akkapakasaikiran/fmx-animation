#include "common.hpp"

GLuint shaderProgram;

glm::mat4 rotation_matrix;
glm::mat4 projection_matrix;
glm::mat4 c_rotation_matrix;
glm::mat4 lookat_matrix;
glm::mat4 model_matrix;
glm::mat4 view_matrix;
glm::mat4 modelview_matrix;

GLuint uModelViewMatrix;
const int num_vertices = 36;

//////////////////////////////////////////////////////////////////////////////////////////

// // Returns an array of eight vertices in homogenous coordinates
// glm::vec4* cube_coords(){
// 	// static to prevent destruction at end of scope
// 	static glm::vec4 positions[8] = {
// 		glm::vec4(-1, -1, 1, 1.0),
// 		glm::vec4(-1, 1, 1, 1.0),
// 		glm::vec4(1, 1, 1, 1.0),
// 		glm::vec4(1, -1, 1, 1.0),
// 		glm::vec4(-1, -1, -1, 1.0),
// 		glm::vec4(-1, 1, -1, 1.0),
// 		glm::vec4(1, 1, -1, 1.0),
// 		glm::vec4(1, -1, -1, 1.0),
// 	};
// 	return positions;
// }

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
void quad(int a, int b, int c, int d, int i, glm::vec4* posns_arr, glm::vec4* positions)
{
	posns_arr[i] = positions[a]; i++;
	posns_arr[i] = positions[b]; i++;
	posns_arr[i] = positions[c]; i++;
	posns_arr[i] = positions[a]; i++;
	posns_arr[i] = positions[c]; i++;
	posns_arr[i] = positions[d]; i++;
}

void initcube(glm::vec4* posns_arr, glm::vec4* positions, int sz){ // sz is always 36
	int i = 0;
	// Making six faces of the cube
	quad(1, 0, 3, 2, i, posns_arr, positions); i += sz/6;
	quad(2, 3, 7, 6, i, posns_arr, positions); i += sz/6;
	quad(3, 0, 4, 7, i, posns_arr, positions); i += sz/6;
	quad(6, 5, 1, 2, i, posns_arr, positions); i += sz/6;
	quad(4, 5, 6, 7, i, posns_arr, positions); i += sz/6;
	quad(5, 4, 0, 1, i, posns_arr, positions); i += sz/6;
}

//////////////////////////////////////////////////////////////////////////////////////////

void initBuffersGL(void)
{
	// Load shaders and use the resulting shader program
	std::string vertex_shader_file("shaders/07_vshader.glsl");
	std::string fragment_shader_file("shaders/07_fshader.glsl");

	std::vector<GLuint> shaderList;
	shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
	shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

	shaderProgram = csX75::CreateProgramGL(shaderList);
	glUseProgram(shaderProgram);

	// Get the attributes from the shader program
	vPosition = glGetAttribLocation(shaderProgram, "vPosition");
	vColor = glGetAttribLocation(shaderProgram, "vColor"); 
	uModelViewMatrix = glGetUniformLocation(shaderProgram, "uModelViewMatrix");

	glm::vec4 positions[8];
	cube_coords(positions);
	glm::vec4 posns_arr[36];
	initcube(posns_arr, positions, 36);

	double rscale = 1;
	glm::vec4 red(1,0,0,0), sepia(0.8,0.5,0.3,0), blue(0.1,0.9,0.1,0);

	nodes["hip"] = new csX75::HNode(nodes["root"], 36, posns_arr, sizeof(posns_arr), red);
	nodes["hip"]->change_parameters(-1.5,0.25,1.5, 0,0,0, 0,0,0, 0,0,0);

	nodes["torso"] = new csX75::HNode(nodes["hip"], 36, posns_arr, sizeof(posns_arr), red);
	nodes["torso"]->change_parameters(0,0,0, 0,0,0, rscale*1.5,rscale*2,rscale*0.5, 0,rscale*2,0);

	nodes["neck"] = new csX75::HNode(nodes["torso"], 36, posns_arr, sizeof(posns_arr), sepia);
	nodes["neck"]->change_parameters(0,rscale*2.5,0, 0,0,0, rscale*0.5,rscale*0.5,rscale*0.5, 0,0,0);
	
	nodes["head"] = new csX75::HNode(nodes["neck"], 36, posns_arr, sizeof(posns_arr), sepia);
	nodes["head"]->change_parameters(0,0,0, 0,0,0, rscale*1,rscale*1,rscale*0.5, 0,rscale*1.5,0);

	nodes["lupper_arm"] = new csX75::HNode(nodes["torso"], 36, posns_arr, sizeof(posns_arr), red);
	nodes["lupper_arm"]->change_parameters(rscale*-1.5,rscale*1.5,0, 0,0,0, rscale*1,rscale*0.5,rscale*0.5, rscale*-1,0,0);

	nodes["llower_arm"] = new csX75::HNode(nodes["lupper_arm"], 36, posns_arr, sizeof(posns_arr), sepia);
	nodes["llower_arm"]->change_parameters(rscale*-1,0,0, 0,0,0, rscale*1,rscale*0.5,rscale*0.5, rscale*-1,0,0);

	nodes["rupper_arm"] = new csX75::HNode(nodes["torso"], 36, posns_arr, sizeof(posns_arr), red);
	nodes["rupper_arm"]->change_parameters(rscale*1.5,rscale*1.5,0, 0,0,0, rscale*1,rscale*0.5,rscale*0.5, rscale*1,0,0);

	nodes["rlower_arm"] = new csX75::HNode(nodes["rupper_arm"], 36, posns_arr, sizeof(posns_arr), sepia);
	nodes["rlower_arm"]->change_parameters(rscale*1,0,0, 0,0,0, rscale*1,rscale*0.5,rscale*0.5, rscale*1,0,0);

	nodes["lupper_leg"] = new csX75::HNode(nodes["hip"], 36, posns_arr, sizeof(posns_arr), glm::vec4(0,0,1,0));
	nodes["lupper_leg"]->change_parameters(rscale*-1,0,0, 0,0,0, rscale*0.5,rscale*1,rscale*0.5, 0,rscale*-1,0);

	nodes["llower_leg"] = new csX75::HNode(nodes["lupper_leg"], 36, posns_arr, sizeof(posns_arr), glm::vec4(0,0,1,0));
	nodes["llower_leg"]->change_parameters(0,rscale*-1,0, 0,0,0, rscale*0.5,rscale*1,rscale*0.5, 0,rscale*-1,0);
	
	nodes["rupper_leg"] = new csX75::HNode(nodes["hip"], 36, posns_arr, sizeof(posns_arr), glm::vec4(0,0,1,0));
	nodes["rupper_leg"]->change_parameters(rscale*1,0,0, 0,0,0, rscale*0.5,rscale*1,rscale*0.5, 0,rscale*-1,0);

	nodes["rlower_leg"] = new csX75::HNode(nodes["rupper_leg"], 36, posns_arr, sizeof(posns_arr), glm::vec4(0,0,1,0));
	nodes["rlower_leg"]->change_parameters(0,rscale*-1,0, 0,0,0, rscale*0.5,rscale*1,rscale*0.5, 0,rscale*-1,0);
	root_node = curr_node = nodes["hip"];
}

void renderGL(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	matrixStack.clear();

	//Creating the lookat and the up vectors for the camera
	c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f,0.0f,0.0f));
	c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f,1.0f,0.0f));
	c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f,0.0f,1.0f));

	glm::vec4 c_pos = glm::vec4(c_xpos,c_ypos,c_zpos, 1.0)*c_rotation_matrix;
	glm::vec4 c_up = glm::vec4(c_up_x,c_up_y,c_up_z, 1.0)*c_rotation_matrix;
	//Creating the lookat matrix
	lookat_matrix = glm::lookAt(glm::vec3(c_pos),glm::vec3(0.0),glm::vec3(c_up));

	//creating the projection matrix
	projection_matrix = glm::frustum(-1.0, 1.0, -1.0, 1.0, 10.0, 500.0);

	view_matrix = projection_matrix * lookat_matrix;

	matrixStack.push_back(view_matrix);

	root_node->render_tree();
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
