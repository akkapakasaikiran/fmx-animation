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

//////////////////////////////////////////////////////////////////////////////////////////

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


// Write a pointset for a circle in a plane parallel to the x-y plane into posns
void circle_points(glm::vec4* posns, int num_pts, double r, double z){
	double angle = glm::radians(360.0)/num_pts; 
	for(int i = 0; i < num_pts; i++)
		posns[i] = glm::vec4(r*glm::cos(i*angle), r*glm::sin(i*angle), z, 1.0);
}

// Constructs a square given four points and puts it in vertices 
void square(glm::vec4* vertices, glm::vec4 a, glm::vec4 b, glm::vec4 c, glm::vec4 d, int i){
	vertices[i++] = a; vertices[i++] = b; vertices[i++] = c;
	vertices[i++] = a; vertices[i++] = c; vertices[i++] = d; 
}

// Fill vertices with a mesh for the curver surface formed by two parallel circles
void bcurved_surface(glm::vec4* vertices, glm::vec4* cl, glm::vec4* cr, int num_pts, int offset){
	for(int i = 0; i < num_pts-1; i++)
		square(vertices, cl[i], cr[i], cr[i+1], cl[i+1], offset+6*i);
	square(vertices, cl[num_pts-1], cr[num_pts-1], cr[0], cl[0], offset+6*num_pts-6);
}

// Fill vertices with a mesh for the annular disc surface formed by two concentric circles
void bannular_disc(glm::vec4* vertices, glm::vec4* lc, glm::vec4* sc, int num_pts, int offset){
	for(int i = 0; i < num_pts-1; i++)
		square(vertices, sc[i], lc[i], lc[i+1], sc[i+1], offset+6*i);
	square(vertices, sc[num_pts-1], lc[num_pts-1], lc[0], sc[0], offset+6*num_pts-6);
}

void spokes(glm::vec4* vertices, glm::vec4* cl, glm::vec4* cr, int num_pts, int num_spokes, int offset){
	for(int i = 0; i < num_spokes; i++)
		square(vertices, cl[i*num_pts/(2*num_spokes)], cr[i*num_pts/(2*num_spokes)],
			cr[(i+num_spokes)*num_pts/(2*num_spokes)], cl[(i+num_spokes)*num_pts/(2*num_spokes)], offset+6*i);
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

/////////////////////////////////////////////////////////////////////////////////
	// Geometry

	glm::vec4 positions[8];
	cube_coords(positions);
	glm::vec4 posns_arr[36];
	initcube(posns_arr, positions, 36);

	int num_bpts = 40;
	int num_spokes = 5;
	double br = 0.75, bR = 1;

	glm::vec4* largecl = new glm::vec4[num_bpts];
	circle_points(largecl, num_bpts, bR, -1);
	glm::vec4* largecr = new glm::vec4[num_bpts];
	circle_points(largecr, num_bpts, bR,  1);
	glm::vec4* smallcl = new glm::vec4[num_bpts];
	circle_points(smallcl, num_bpts, br, -1);
	glm::vec4* smallcr = new glm::vec4[num_bpts];
	circle_points(smallcr, num_bpts, br,  1);

	int num_bvertices = 4*6*num_bpts;
	glm::vec4 bvertices[num_bvertices];
	bcurved_surface(bvertices, largecl, largecr, num_bpts, 0);
	bcurved_surface(bvertices, smallcl, smallcr, num_bpts, 6*num_bpts);

	bannular_disc(bvertices, largecr, smallcr, num_bpts, 2*6*num_bpts);
	bannular_disc(bvertices, largecl, smallcl, num_bpts, 3*6*num_bpts);

	glm::vec4 spokes_arr[6*num_spokes];
	spokes(spokes_arr, smallcl, smallcr, num_bpts, num_spokes, 0);

	double bscale = 1;
	glm::vec4 tire_color(0.2,0.2,0.2,0), seat_color(0.1,0.5,0.6,0),
			handle_color(0.3,0.9,0.2,0), engine_color(0.6,0.8,0.9,0),
			front_rod_color(0.6,0.8,0.9,0), handlebar_color(0.2,0.3,0.5,0),
			spokes_color(.6,.65,.7,0), frontlight_color(1.0,1.0,1.0,0),
			body1_color(0.9,0.1,0.1,0), grip_color(0.85,0.75,0.2,0);

	///////////////////////////////////////////////////////////////////////////
	// Hierarchical Model

	nodes["engine"] = new csX75::HNode(NULL, 36, posns_arr, sizeof(posns_arr), engine_color);
	nodes["engine"]->change_parameters(0,bscale*-1,0, 0,0,0, bscale*0.4,bscale*0.4,bscale*0.4, 0,0,0);

	nodes["back_rod1"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), front_rod_color);
	nodes["back_rod1"]->change_parameters(-bscale*3.1,-bscale*0.9,bscale*0.35, 0,0,15, bscale*1.5,bscale*0.1,bscale*0.1, bscale*1.5,0,0);
	nodes["back_rod2"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), front_rod_color);
	nodes["back_rod2"]->change_parameters(-bscale*3.1,-bscale*0.9,-bscale*0.35, 0,0,15, bscale*1.5,bscale*0.1,bscale*0.1, bscale*1.5,0,0);

	nodes["frontlight"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), frontlight_color);
	nodes["frontlight"]->change_parameters(0,bscale*0.5,0, 0,0,25, bscale*0.2,bscale*0.4,bscale*0.4, bscale*3,0,0);
	nodes["body1"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), body1_color);
	nodes["body1"]->change_parameters(0,bscale*0.5,0, 0,0,25, bscale*1.4,bscale*0.5,bscale*0.5, bscale*1.5,0,0);
	nodes["body2"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), seat_color);
	nodes["body2"]->change_parameters(0,bscale*0.5,0, 0,0,-10, bscale*1.4,bscale*0.45,bscale*0.5, -bscale*1.5,0,0);

	nodes["handlebar"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), handlebar_color);
	nodes["handlebar"]->change_parameters(bscale*2,bscale*3,0, 0,0,0, bscale*0.14,bscale*0.14,bscale*1.8, 0,0,0);
	nodes["grip1"] = new csX75::HNode(nodes["handlebar"], 36, posns_arr, sizeof(posns_arr), grip_color);
	nodes["grip1"]->change_parameters(0,0,bscale*1.2, 0,0,0, bscale*0.17,bscale*0.17,bscale*0.5, 0,0,0);
	nodes["grip2"] = new csX75::HNode(nodes["handlebar"], 36, posns_arr, sizeof(posns_arr), grip_color);
	nodes["grip2"]->change_parameters(0,0,-bscale*1.2, 0,0,0, bscale*0.17,bscale*0.17,bscale*0.5, 0,0,0);

	nodes["front_rod1"] = new csX75::HNode(nodes["handlebar"], 36, posns_arr, sizeof(posns_arr), front_rod_color);
	nodes["front_rod1"]->change_parameters(0,0,bscale*0.35, 0,0,15, bscale*0.1,bscale*2.2,bscale*0.1, 0,bscale*-2,0);
	nodes["front_rod2"] = new csX75::HNode(nodes["handlebar"], 36, posns_arr, sizeof(posns_arr), front_rod_color);
	nodes["front_rod2"]->change_parameters(0,0,-bscale*0.35, 0,0,15, bscale*0.1,bscale*2.2,bscale*0.1, 0,bscale*-2,0);

	nodes["back_tire"] = new csX75::HNode(nodes["engine"], num_bvertices, bvertices, sizeof(bvertices), tire_color);
	nodes["back_tire"]->change_parameters(bscale*-3,bscale*-1,0, 0,0,0, bscale*1.5,bscale*1.5,bscale*0.245, 0,0,0);

	nodes["bspokes"] = new csX75::HNode(nodes["back_tire"], 6*num_spokes, spokes_arr, sizeof(spokes_arr), spokes_color);
	nodes["bspokes"]->change_parameters(0,0,0, 0,0,0, bscale*1.5,bscale*1.5,bscale*0.245, 0,0,0);	

	nodes["front_tire"] = new csX75::HNode(nodes["handlebar"], num_bvertices, bvertices, sizeof(bvertices), tire_color);
	nodes["front_tire"]->change_parameters(bscale*1,bscale*-4,0, 0,0,0, bscale* 1.5,bscale* 1.5,bscale*0.245, 0,0,0);

	nodes["fspokes"] = new csX75::HNode(nodes["front_tire"], 6*num_spokes, spokes_arr, sizeof(spokes_arr), spokes_color);
	nodes["fspokes"]->change_parameters(0,0,0, 0,0,0, bscale*1.5,bscale*1.5,bscale*0.245, 0,0,0);	



	root_node = curr_node = nodes["engine"];
}

void renderGL(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	matrixStack.clear();

	c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f,0.0f,0.0f));
	c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f,1.0f,0.0f));
	c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f,0.0f,1.0f));

	glm::vec4 c_pos = glm::vec4(c_xpos,c_ypos,c_zpos, 1.0)*c_rotation_matrix;
	glm::vec4 c_up = glm::vec4(c_up_x,c_up_y,c_up_z, 1.0)*c_rotation_matrix;
	lookat_matrix = glm::lookAt(glm::vec3(c_pos),glm::vec3(0.0),glm::vec3(c_up));

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
