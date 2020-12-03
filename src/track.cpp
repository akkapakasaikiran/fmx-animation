#include "common.hpp"
#include<unistd.h>
#include<fstream>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_TGA
#include "stb_image_read.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.hpp"


#include <iostream>

GLuint shaderProgram;

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
GLuint headlight;

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
void quad(int a, int b, int c, int d, int i, glm::vec4* normals_arr, glm::vec4* posns_arr, glm::vec4* positions)
{

	glm::vec3 v1=glm::vec3(positions[b].x - positions[a].x, positions[b].y - positions[a].y, positions[b].z - positions[a].z);
	glm::vec3 v2=glm::vec3(positions[c].x - positions[a].x, positions[c].y - positions[a].y, positions[c].z - positions[a].z);

	glm::vec3 norml=glm::cross(v1,v2);
	//glm::vec3 norml=glm::cross(v2,v1);
	glm::vec4 normal=glm::vec4(norml.x, norml.y, norml.z, 1.0);

	posns_arr[i] = positions[a]; normals_arr[i] = normal; i++;
	posns_arr[i] = positions[b]; normals_arr[i] = normal; i++;
	posns_arr[i] = positions[c]; normals_arr[i] = normal; i++;
	posns_arr[i] = positions[a]; normals_arr[i] = normal; i++;
	posns_arr[i] = positions[c]; normals_arr[i] = normal; i++;
	posns_arr[i] = positions[d]; normals_arr[i] = normal; i++;
}

void initcube(glm::vec4* normals_arr, glm::vec4* posns_arr, glm::vec4* positions, int sz){ // sz is always 36
	int i = 0;
	// Making six faces of the cube
	quad(1, 0, 3, 2, i, normals_arr, posns_arr, positions); i += sz/6;
	quad(2, 3, 7, 6, i, normals_arr, posns_arr, positions); i += sz/6;
	quad(3, 0, 4, 7, i, normals_arr, posns_arr, positions); i += sz/6;
	quad(6, 5, 1, 2, i, normals_arr, posns_arr, positions); i += sz/6;
	quad(4, 5, 6, 7, i, normals_arr, posns_arr, positions); i += sz/6;
	quad(5, 4, 0, 1, i, normals_arr, posns_arr, positions); i += sz/6;
}

///////////////////////////////////////////////////////////////////////////////////////////

// Write a pointset for a circle in a plane parallel to the x-y plane into posns
void circle_points(glm::vec4* posns, int num_pts, double r, double z){
	double angle = glm::radians(360.0)/num_pts; 
	for(int i = 0; i < num_pts; i++)
		posns[i] = glm::vec4(r*glm::cos(i*angle), r*glm::sin(i*angle), z, 1.0);
}

// Constructs a square given four points and puts it in vertices 
void square(glm::vec4* normals_arr, glm::vec4* vertices, glm::vec4 a, glm::vec4 b, glm::vec4 c, glm::vec4 d, int i){

	glm::vec3 v1=glm::vec3(b.x - a.x, b.y - a.y, b.z - a.z);
	glm::vec3 v2=glm::vec3(c.x - a.x, c.y - a.y, c.z - a.z);

	//glm::vec3 norml=glm::cross(v1,v2);
	glm::vec3 norml=glm::cross(v2,v1);
	glm::vec4 normal=glm::vec4(norml.x, norml.y, norml.z, 1.0);

	normals_arr[i] = normal; vertices[i++] = a;
	normals_arr[i] = normal; vertices[i++] = b;
	normals_arr[i] = normal; vertices[i++] = c;
	normals_arr[i] = normal; vertices[i++] = a; 
	normals_arr[i] = normal; vertices[i++] = c; 
	normals_arr[i] = normal; vertices[i++] = d; 
}

// Fill vertices with a mesh for the curved surface formed by two parallel circles
void curved_surface(glm::vec4* normals_arr, glm::vec4* vertices, glm::vec4* cl, glm::vec4* cr, int num_pts, int offset){
	for(int i = 0; i < num_pts-1; i++)
		square(normals_arr, vertices, cl[i], cr[i], cr[i+1], cl[i+1], offset+6*i);
	// square(normals_arr, vertices, cl[num_pts-1], cr[num_pts-1], cr[0], cl[0], offset+6*num_pts-6);

}

// Fill vertices with a mesh for the annular disc surface formed by two concentric circles
void annular_disc(glm::vec4* normals_arr, glm::vec4* vertices, glm::vec4* lc, glm::vec4* sc, int num_pts, int offset){
	for(int i = 0; i < num_pts-1; i++)
		square(normals_arr, vertices, sc[i], lc[i], lc[i+1], sc[i+1], offset+6*i);
	// square(normals_arr, vertices, sc[num_pts-1], lc[num_pts-1], lc[0], sc[0], offset+6*num_pts-6);
}


///////////////////////////////////////////////////////////////////////////////////////////


// Fill vertices with a mesh for the curved surface formed by two parallel circles
void bcurved_surface(glm::vec4* normals_arr, glm::vec4* vertices, glm::vec4* cl, glm::vec4* cr, int num_pts, int offset){
	for(int i = 0; i < num_pts-1; i++)
		square(normals_arr, vertices, cl[i], cr[i], cr[i+1], cl[i+1], offset+6*i);
	square(normals_arr, vertices, cl[num_pts-1], cr[num_pts-1], cr[0], cl[0], offset+6*num_pts-6);
}

// Fill vertices with a mesh for the annular disc surface formed by two concentric circles
void bannular_disc(glm::vec4* normals_arr, glm::vec4* vertices, glm::vec4* lc, glm::vec4* sc, int num_pts, int offset){
	for(int i = 0; i < num_pts-1; i++)
		square(normals_arr, vertices, sc[i], lc[i], lc[i+1], sc[i+1], offset+6*i);
	square(normals_arr, vertices, sc[num_pts-1], lc[num_pts-1], lc[0], sc[0], offset+6*num_pts-6);
}

void spokes(glm::vec4* normals_arr, glm::vec4* vertices, glm::vec4* cl, glm::vec4* cr, int num_pts, int num_spokes, int offset){
	for(int i = 0; i < num_spokes; i++)
		square(normals_arr, vertices, cl[i*num_pts/(2*num_spokes)], cr[i*num_pts/(2*num_spokes)],
			cr[(i+num_spokes)*num_pts/(2*num_spokes)], cl[(i+num_spokes)*num_pts/(2*num_spokes)], offset+6*i);
}


///////////////////////////////////////////////////////////////////////////////////////////


void quad_coords(glm::vec4* positions){
	positions[0] = glm::vec4(-5, 0, 0, 1.0);
	positions[1] = glm::vec4(-10, 0, 0, 1.0);
	positions[2] = glm::vec4(-10, 5, 0, 1.0);
	positions[3] = glm::vec4(-5, 5, 0, 1.0);
}

void rectangle(glm::vec4* normals_arr, glm::vec4* vertices, int num_vertices){
	double divisions = num_vertices/6.0;

	double x_l = -0.5;
	double x_r = 0.5;
	double z_l = 0;
	double z_r = 0;

	double diff_z = 3.0/divisions;

	glm::vec4 positions[4];

	for(int i=0;i<num_vertices;i+=6){
		//std::cout<<"hi\n";
		positions[0] = glm::vec4(x_l, 0, z_r, 1.0);
		positions[1] = glm::vec4(x_r, 0, z_l, 1.0);
		z_l = z_l + diff_z;
		z_r = z_r + diff_z;
		positions[2] = glm::vec4(x_r, 0, z_l, 1.0);
		positions[3] = glm::vec4(x_l, 0, z_r, 1.0);
		square(normals_arr, vertices, positions[0], positions[1], positions[2], positions[3], i);
	}
}

void semi_circle_points(glm::vec4* posns, int num_pts, double r, double y){
	double angle = glm::radians(180.0)/num_pts; 
	for(int i = 0; i <= num_pts; i++)
		posns[i] = glm::vec4(r*glm::cos(i*angle), y, r*glm::sin(i*angle), 1.0);
}

void gaussian_points(glm::vec4* posns, int num_pts, double x, int mult){ 
	posns[0] = glm::vec4(x,0,-1,1.0);
	double z = -1;
	for(int i = 1; i < num_pts-1; i++){
		z = z + (0.1*mult);
		posns[i] = glm::vec4(x, glm::exp(-4.5*z*z), z, 1.0);
	}
	posns[num_pts - 1] = glm::vec4(x,0,1,1.0);
}

void half_gaussian_points(glm::vec4* posns, int num_pts, double x){ 
	posns[0] = glm::vec4(x,0,-1,1.0);
	double z = -1;
	for(int i = 1; i < num_pts; i++){
		z = z + 0.1;
		posns[i] = glm::vec4(x, glm::exp(-4.5*z*z), z, 1.0);
	}
	//posns[num_pts - 1] = glm::vec4(x,0,1,1.0);
}

void laplacian_points(glm::vec4* posns, int num_pts, double x, int mult){ 
	posns[0] = glm::vec4(x,0,-1,1.0);
	double z = -1;
	for(int i = 1; i < num_pts-1; i++){
		z = z + (0.1*mult);

		int sign=-1;
		if(z>0)
			sign=1;

		posns[i] = glm::vec4(x, glm::exp(-4*z*sign), z, 1.0);
	}
	posns[num_pts - 1] = glm::vec4(x,0,1,1.0);
}

//////////////////////////////////////////////////////////////////////////////////////////

void initBuffersGL(void)
{
	// Load shaders and use the resulting shader program
	std::string vertex_shader_file("shaders/05_vshader.glsl");
	std::string fragment_shader_file("shaders/05_fshader.glsl");

	std::vector<GLuint> shaderList;
	shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
	shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

	shaderProgram = csX75::CreateProgramGL(shaderList);
	glUseProgram(shaderProgram);

	// Get the attributes from the shader program
	vPosition = glGetAttribLocation(shaderProgram, "vPosition");
	vColor = glGetAttribLocation(shaderProgram, "vColor"); 
	vNormal = glGetAttribLocation( shaderProgram, "vNormal" ); 
	MVP = glGetUniformLocation(shaderProgram, "MVP");
	normalMatrix =  glGetUniformLocation( shaderProgram, "normalMatrix");
  	ModelviewMatrix = glGetUniformLocation( shaderProgram, "ModelviewMatrix");
  	light_stat = glGetUniformLocation( shaderProgram, "light_stat");
  	headlight = glGetUniformLocation( shaderProgram, "headlight");


	int num_vertices = 60;

	glm::vec4 vertices[num_vertices];
	glm::vec4 normals[num_vertices];

	rectangle(normals, vertices,num_vertices);

/////////////////////////////////////////////////////////////////////////////////////////////
	// Main Root, a dummy node

	nodes["root"] = new csX75::HNode(NULL, 60, vertices , sizeof(vertices), glm::vec4(1,0,0,1), normals);
	nodes["root"]->change_parameters(0,0,0, 0,0,0, 0,0,0, -5.5,0,0);
	// pre rotation translation added to ensure rotation of track occurs aobut the centroid

	root_node = curr_node = nodes["root"];

/////////////////////////////////////////////////////////////////////////////////////////////
	// Track

	// Geometry

	int num_pts = 20;
	double r = 1, R = 2;
	glm::vec4* largec = new glm::vec4[num_pts+1];
	semi_circle_points(largec, num_pts, R, 0);
	glm::vec4* smallc = new glm::vec4[num_pts+1];
	semi_circle_points(smallc, num_pts, r, 0);

	glm::vec4 semicircle[6*num_pts+6];
	glm::vec4 semicircle_normals[6*num_pts+6];
	annular_disc(semicircle_normals, semicircle, largec, smallc, num_pts+1, 0);

	int num_expo = 21;
	double x=0.5;
	glm::vec4* right = new glm::vec4[num_expo];
	gaussian_points(right, num_expo, x, 1);

	x=-0.5;
	glm::vec4* left = new glm::vec4[num_expo];
	gaussian_points(left, num_expo, x, 1);

	glm::vec4 obstacle1[6*num_expo-6];
	glm::vec4 obstacle1_normals[6*num_expo-6];
	curved_surface(obstacle1_normals, obstacle1, left, right, num_expo, 0);

	num_expo = 11;
	x=0.5;
	glm::vec4* right1 = new glm::vec4[num_expo];
	half_gaussian_points(right1, num_expo, x);

	x=-0.5;
	glm::vec4* left1 = new glm::vec4[num_expo];
	half_gaussian_points(left1, num_expo, x);

	glm::vec4 obstacle2[6*num_expo-6];
	glm::vec4 obstacle2_normals[6*num_expo-6];
	curved_surface(obstacle2_normals, obstacle2, left1, right1, num_expo, 0);

	num_expo = 3;
	x=0.5;
	glm::vec4* right2 = new glm::vec4[num_expo];
	gaussian_points(right2, num_expo, x, 10);

	x=-0.5;
	glm::vec4* left2 = new glm::vec4[num_expo];
	gaussian_points(left2, num_expo, x, 10);

	glm::vec4 obstacle3[6*num_expo-6];
	glm::vec4 obstacle3_normals[6*num_expo-6];
	curved_surface(obstacle3_normals, obstacle3, left2, right2, num_expo, 0);

	num_expo = 21;
	x=0.5;
	glm::vec4* right3 = new glm::vec4[num_expo];
	laplacian_points(right3, num_expo, x, 1);

	x=-0.5;
	glm::vec4* left3 = new glm::vec4[num_expo];
	laplacian_points(left3, num_expo, x, 1);

	glm::vec4 obstacle4[6*num_expo-6];
	glm::vec4 obstacle4_normals[6*num_expo-6];
	curved_surface(obstacle4_normals, obstacle4, left3, right3, num_expo, 0);

	// Hierarchical Model

	nodes["rect1"] = new csX75::HNode(nodes["root"], 60, vertices , sizeof(vertices), glm::vec4(1,0,0,0), normals);
	nodes["rect1"]->change_parameters(-1.5,0,0, 0,0,0, 1,1,1, 0,0,0);

	nodes["semicircle"] = new csX75::HNode(nodes["rect1"], 120, semicircle , sizeof(semicircle), glm::vec4(1,0,0,0), semicircle_normals);
	nodes["semicircle"]->change_parameters(1.5,0,3, 0,0,0, 1,1,1, 0,0,0);

	nodes["rect2"] = new csX75::HNode(nodes["semicircle"], 60, vertices , sizeof(vertices), glm::vec4(1,0,0,0), normals);
	nodes["rect2"]->change_parameters(1.5,0,-3, 0,0,0, 1,1,1, 0,0,0);

	nodes["obstacle1"] = new csX75::HNode(nodes["rect2"], 120, obstacle1 , sizeof(obstacle1), glm::vec4(1,0,0,0), obstacle1_normals);
	nodes["obstacle1"]->change_parameters(0,0,-1, 0,0,0, 1,1,1, 0,0,0);

	nodes["rect3"] = new csX75::HNode(nodes["obstacle1"], 60, vertices , sizeof(vertices), glm::vec4(1,0,0,0), normals);
	nodes["rect3"]->change_parameters(0,0,-4, 0,0,0, 1,1,1, 0,0,0);

	nodes["semicircle2"] = new csX75::HNode(nodes["rect3"], 120, semicircle , sizeof(semicircle), glm::vec4(1,0,0,0), semicircle_normals);
	nodes["semicircle2"]->change_parameters(1.5,0,0, 0,0,0, -1,1,-1, 0,0,0);

	nodes["rect4"] = new csX75::HNode(nodes["semicircle2"], 60, vertices , sizeof(vertices), glm::vec4(1,0,0,0), normals);
	nodes["rect4"]->change_parameters(1.5,0,0, 0,0,0, 1,1,0.5, 0,0,0);

	nodes["obstacle2_1"] = new csX75::HNode(nodes["rect4"], 60, obstacle2 , sizeof(obstacle2), glm::vec4(1,0,0,0), obstacle2_normals);
	nodes["obstacle2_1"]->change_parameters(0,0,2.5, 0,0,0, 1,1,1, 0,0,0);

	nodes["obstacle2_2"] = new csX75::HNode(nodes["obstacle2_1"], 60, obstacle2 , sizeof(obstacle2), glm::vec4(1,0,0,0), obstacle2_normals);
	nodes["obstacle2_2"]->change_parameters(0,0,1, 0,0,0, 1,1,-1, 0,0,0);

	nodes["rect5"] = new csX75::HNode(nodes["obstacle2_2"], 60, vertices , sizeof(vertices), glm::vec4(1,0,0,0), normals);
	nodes["rect5"]->change_parameters(0,0,1, 0,0,0, 1,1,0.5, 0,0,0);

	nodes["obstacle3_1"] = new csX75::HNode(nodes["rect5"], 12, obstacle3 , sizeof(obstacle2), glm::vec4(1,0,0,0), obstacle3_normals);
	nodes["obstacle3_1"]->change_parameters(0,0,1.6, 0,0,0, 1,0.1,0.1, 0,0,0);

	nodes["obstacle3_2"] = new csX75::HNode(nodes["obstacle3_1"], 12, obstacle3 , sizeof(obstacle2), glm::vec4(1,0,0,0), obstacle3_normals);
	nodes["obstacle3_2"]->change_parameters(0,0,0.2, 0,0,0, 1,0.1,0.1, 0,0,0);

	nodes["obstacle3_3"] = new csX75::HNode(nodes["obstacle3_2"], 12, obstacle3 , sizeof(obstacle2), glm::vec4(1,0,0,0), obstacle3_normals);
	nodes["obstacle3_3"]->change_parameters(0,0,0.2, 0,0,0, 1,0.1,0.1, 0,0,0);

	nodes["obstacle3_4"] = new csX75::HNode(nodes["obstacle3_3"], 12, obstacle3 , sizeof(obstacle2), glm::vec4(1,0,0,0), obstacle3_normals);
	nodes["obstacle3_4"]->change_parameters(0,0,0.2, 0,0,0, 1,0.1,0.1, 0,0,0);

	nodes["rect6"] = new csX75::HNode(nodes["obstacle3_4"], 60, vertices , sizeof(vertices), glm::vec4(1,0,0,0), normals);
	nodes["rect6"]->change_parameters(0,0,0.1, 0,0,0, 1,1,0.5, 0,0,0);

	nodes["semicircle3"] = new csX75::HNode(nodes["rect6"], 120, semicircle , sizeof(semicircle), glm::vec4(1,0,0,0), semicircle_normals);
	nodes["semicircle3"]->change_parameters(1.5,0,1.5, 0,0,0, 1,1,1, 0,0,0);

	nodes["rect7"] = new csX75::HNode(nodes["semicircle3"], 60, vertices , sizeof(vertices), glm::vec4(1,0,0,0), normals);
	nodes["rect7"]->change_parameters(1.5,0,0, 0,0,0, -1,1,-1, 0,0,0);

	nodes["obstacle4"] = new csX75::HNode(nodes["rect7"], 120, obstacle4 , sizeof(obstacle4), glm::vec4(1,0,0,0), obstacle4_normals);
	nodes["obstacle4"]->change_parameters(0,0,-4, 0,0,0, 1,1,1, 0,0,0);

	nodes["rect8"] = new csX75::HNode(nodes["obstacle4"], 60, vertices , sizeof(vertices), glm::vec4(1,0,0,0), normals);
	nodes["rect8"]->change_parameters(0,0,-1, 0,0,0, -1,1,-1, 0,0,0);

	nodes["semicircle4"] = new csX75::HNode(nodes["rect8"], 120, semicircle , sizeof(semicircle), glm::vec4(1,0,0,0), semicircle_normals);
	nodes["semicircle4"]->change_parameters(1.5,0,-3, 0,0,0, -1,1,-1, 0,0,0);

	nodes["rect9"] = new csX75::HNode(nodes["semicircle4"], 60, vertices , sizeof(vertices), glm::vec4(1,0,0,0), normals);
	nodes["rect9"]->change_parameters(1.5,0,0, 0,0,0, 1,1,1, 0,0,0);

	glm::vec4 obstacle1_normals_copy[6*num_expo-6];
	for(int i=0;i<6*num_expo-6;i++)
		obstacle1_normals_copy[i] = glm::vec4((-1)*obstacle1_normals[i][0], (-1)*obstacle1_normals[i][1], (-1)*obstacle1_normals[i][2], (-1)*obstacle1_normals[i][3]); //inverted gaussian, normals should be in the opposite direction

	nodes["obstacle5"] = new csX75::HNode(nodes["rect9"], 120, obstacle1 , sizeof(obstacle1), glm::vec4(1,0,0,0), obstacle1_normals_copy);
	nodes["obstacle5"]->change_parameters(0,0,4, 0,0,0, 1,-1,1, 0,0,0);

	nodes["rect10"] = new csX75::HNode(nodes["obstacle5"], 60, vertices , sizeof(vertices), glm::vec4(1,0,0,0), normals);
	nodes["rect10"]->change_parameters(0,0,1, 0,0,0, 1,1,1, 0,0,0);

/////////////////////////////////////////////////////////////////////////////////////////////
	// Rider

	// Geometry

	// posns_array used in both rider and bike
	glm::vec4 positions[8];
	cube_coords(positions);
	glm::vec4 posns_arr[36];
	glm::vec4 normals_arr[36];
	initcube(normals_arr, posns_arr, positions, 36);

	double rscale = 0.045;
	glm::vec4 red(1,0,0,0), sepia(0.8,0.5,0.3,0), blue(0.1,0.9,0.1,0);

	// Hierarchical Model

	nodes["hip"] = new csX75::HNode(nodes["root"], 36, posns_arr, sizeof(posns_arr), red, normals_arr);
	nodes["hip"]->change_parameters(-1.5,0.25,1.5, 0,0,0, 0,0,0, 0,0,0);

	nodes["torso"] = new csX75::HNode(nodes["hip"], 36, posns_arr, sizeof(posns_arr), glm::vec4(0,1,0,0), normals_arr);
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


/////////////////////////////////////////////////////////////////////////////////////////////
	// Bike

	// Geometry

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
	glm::vec4 bnormals[num_bvertices];
	bcurved_surface(bnormals, bvertices, largecl, largecr, num_bpts, 0);
	bcurved_surface(bnormals, bvertices, smallcl, smallcr, num_bpts, 6*num_bpts);

	bannular_disc(bnormals, bvertices, largecr, smallcr, num_bpts, 2*6*num_bpts);
	bannular_disc(bnormals, bvertices, largecl, smallcl, num_bpts, 3*6*num_bpts);

	glm::vec4 spokes_arr[6*num_spokes];
	glm::vec4 spokes_normals[6*num_spokes];
	spokes(spokes_normals, spokes_arr, smallcl, smallcr, num_bpts, num_spokes, 0);

	double bscale = 0.05;
	glm::vec4 tire_color(0.2,0.2,0.2,0), seat_color(0.1,0.5,0.6,0),
			handle_color(0.3,0.9,0.2,0), engine_color(0.6,0.8,0.9,0),
			front_rod_color(0.6,0.8,0.9,0), handlebar_color(0.2,0.3,0.5,0),
			spokes_color(.6,.65,.7,0), frontlight_color(1.0,1.0,1.0,0),
			body1_color(0.9,0.1,0.1,0), grip_color(0.85,0.75,0.2,0);

	///////////////////////////////////////////////////////////////////////////
	// Hierarchical Model

	nodes["engine"] = new csX75::HNode(nodes["root"], 36, posns_arr, sizeof(posns_arr), engine_color, normals_arr);
	nodes["engine"]->change_parameters(-1.5,0.25+bscale*-1,1.00, 0,0,0, bscale*0.4,bscale*0.4,bscale*0.4, 0,0,0);

	nodes["back_rod1"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), front_rod_color, normals_arr);
	nodes["back_rod1"]->change_parameters(-bscale*3.1,-bscale*0.9,bscale*0.35, 0,0,15, bscale*1.5,bscale*0.1,bscale*0.1, bscale*1.5,0,0);
	nodes["back_rod2"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), front_rod_color, normals_arr);
	nodes["back_rod2"]->change_parameters(-bscale*3.1,-bscale*0.9,-bscale*0.35, 0,0,15, bscale*1.5,bscale*0.1,bscale*0.1, bscale*1.5,0,0);

	nodes["frontlight"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), frontlight_color, normals_arr);
	nodes["frontlight"]->change_parameters(0,bscale*0.5,0, 0,0,25, bscale*0.2,bscale*0.4,bscale*0.4, bscale*3,0,0);
	nodes["body1"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), body1_color, normals_arr);
	nodes["body1"]->change_parameters(0,bscale*0.5,0, 0,0,25, bscale*1.4,bscale*0.5,bscale*0.5, bscale*1.5,0,0);
	nodes["body2"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), seat_color, normals_arr);
	nodes["body2"]->change_parameters(0,bscale*0.5,0, 0,0,-10, bscale*1.4,bscale*0.45,bscale*0.5, -bscale*1.5,0,0);

	nodes["handlebar"] = new csX75::HNode(nodes["engine"], 36, posns_arr, sizeof(posns_arr), handlebar_color, normals_arr);
	nodes["handlebar"]->change_parameters(bscale*2,bscale*3,0, 0,0,0, bscale*0.14,bscale*0.14,bscale*1.8, 0,0,0);
	nodes["grip1"] = new csX75::HNode(nodes["handlebar"], 36, posns_arr, sizeof(posns_arr), grip_color, normals_arr);
	nodes["grip1"]->change_parameters(0,0,bscale*1.2, 0,0,0, bscale*0.17,bscale*0.17,bscale*0.5, 0,0,0);
	nodes["grip2"] = new csX75::HNode(nodes["handlebar"], 36, posns_arr, sizeof(posns_arr), grip_color, normals_arr);
	nodes["grip2"]->change_parameters(0,0,-bscale*1.2, 0,0,0, bscale*0.17,bscale*0.17,bscale*0.5, 0,0,0);

	nodes["front_rod1"] = new csX75::HNode(nodes["handlebar"], 36, posns_arr, sizeof(posns_arr), front_rod_color, normals_arr);
	nodes["front_rod1"]->change_parameters(0,0,bscale*0.35, 0,0,15, bscale*0.1,bscale*2.2,bscale*0.1, 0,bscale*-2,0);
	nodes["front_rod2"] = new csX75::HNode(nodes["handlebar"], 36, posns_arr, sizeof(posns_arr), front_rod_color, normals_arr);
	nodes["front_rod2"]->change_parameters(0,0,-bscale*0.35, 0,0,15, bscale*0.1,bscale*2.2,bscale*0.1, 0,bscale*-2,0);

	nodes["back_tire"] = new csX75::HNode(nodes["engine"], num_bvertices, bvertices, sizeof(bvertices), tire_color, bnormals);
	nodes["back_tire"]->change_parameters(bscale*-3,bscale*-1,0, 0,0,0, bscale*1.5,bscale*1.5,bscale*0.245, 0,0,0);

	nodes["bspokes"] = new csX75::HNode(nodes["back_tire"], 6*num_spokes, spokes_arr, sizeof(spokes_arr), spokes_color, spokes_normals);
	nodes["bspokes"]->change_parameters(0,0,0, 0,0,0, bscale*1.5,bscale*1.5,bscale*0.245, 0,0,0);	

	nodes["front_tire"] = new csX75::HNode(nodes["handlebar"], num_bvertices, bvertices, sizeof(bvertices), tire_color, bnormals);
	nodes["front_tire"]->change_parameters(bscale*1,bscale*-4,0, 0,0,0, bscale* 1.5,bscale* 1.5,bscale*0.245, 0,0,0);

	nodes["fspokes"] = new csX75::HNode(nodes["front_tire"], 6*num_spokes, spokes_arr, sizeof(spokes_arr), spokes_color, spokes_normals);
	nodes["fspokes"]->change_parameters(0,0,0, 0,0,0, bscale*1.5,bscale*1.5,bscale*0.245, 0,0,0);	


}


glm::mat4 loadCameras(void){

	glm::vec3 posn(c_xpos, c_ypos, c_zpos);
	// glm::vec3 posn(0.0, 0.0, 50.0);
	glm::vec3 rot(c_xrot, c_yrot, c_zrot);
	glm::vec3 up(c_up_x, c_up_y, c_up_z);
	glm::vec3 lookat_pt(0.0, 0.0, 0.0);
	
	cameras["global"] = new csX75::Camera(posn, rot, up, lookat_pt);

	glm::mat4 mult = nodes["root"]->get_transformation();
	mult *= nodes["hip"]->get_transformation();
	lookat_pt = glm::vec3(mult * glm::vec4(glm::vec3(0.0), 1.0));
	posn = glm::vec3(lookat_pt + glm::vec3(20.0, 10.0, 10.0));

	cameras["third_person"] = new csX75::Camera(posn, rot, up, lookat_pt);

	mult = nodes["root"]->get_transformation();
	mult *= nodes["hip"]->get_transformation();
	mult *= nodes["torso"]->get_transformation();
	mult *= nodes["neck"]->get_transformation();
	mult *= nodes["head"]->get_transformation();

	posn = glm::vec3(mult * glm::vec4(0.0, 0.0, 0.0, 1.0));
	lookat_pt = glm::vec3(mult * glm::vec4(0.0, 0.0, 1.0, 1.0));

	//std::cout<<glm::to_string(posn)<<glm::to_string(lookat_pt)<<"\n";

	cameras["go_pro"] = new csX75::Camera(posn, rot, up, lookat_pt);

	mult = nodes["root"]->get_transformation();
	mult *= nodes["engine"]->get_transformation();
	mult *= nodes["frontlight"]->get_transformation();
	posn = glm::vec3(mult * glm::vec4(0.0, 0.0, 0.0, 1.0)); //position of the headlight
	glUniform4fv(light_stat, 1, glm::value_ptr(posn));

	// camera_num is defined in common.hpp 
	if(camera_num == 0){
		curr_camera = cameras["global"];
		return glm::frustum(-1.0, 1.0, -1.0, 1.0, 10.0, 500.0);
	}
	else if(camera_num == 1){
		curr_camera = cameras["third_person"];
		return glm::frustum(-0.2, 0.2, -0.2, 0.2, 2.0, 100.0);
	}
	else if(camera_num == 2){
		curr_camera = cameras["go_pro"];
		// return glm::ortho(-20.0, 20.0, -20.0, 20.0, -20.0, 20.0); 
		return glm::frustum(-0.02, 0.02, -0.02, 0.02, 0.025, 10.0);
	}

}

void renderGL(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	matrixStack.clear();
	matrixStack1.clear();

	projection_matrix = loadCameras();

	c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(curr_camera->rotation.x), glm::vec3(1.0f,0.0f,0.0f));
	c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(curr_camera->rotation.y), glm::vec3(0.0f,1.0f,0.0f));
	c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(curr_camera->rotation.z), glm::vec3(0.0f,0.0f,1.0f));

	glm::vec4 c_pos = glm::vec4(curr_camera->position, 1.0) * c_rotation_matrix;
	glm::vec4 c_up = glm::vec4(curr_camera->up, 1.0) * c_rotation_matrix;
	lookat_matrix = glm::lookAt(glm::vec3(c_pos), curr_camera->lookat_pt, glm::vec3(c_up));

	view_matrix = projection_matrix * lookat_matrix;

	matrixStack.push_back(view_matrix);
	matrixStack1.push_back(lookat_matrix);

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
	window = glfwCreateWindow(1000, 1000, "FMX Animation", NULL, NULL);
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

		if(mode || mode1 || mode2){
			int count=1; // number of keyframes read
			std::ifstream fs;
			fs.open("keyframes.txt");
			int t;

			fs>>t;

			std::map<std::string, csX75::HNode*>::iterator itr = nodes.begin();

			int camera_num1[2];
			GLfloat c_xpos1[2];
			GLfloat c_ypos1[2];
			GLfloat c_zpos1[2];
			GLfloat c_xrot1[2];
			GLfloat c_yrot1[2];
			GLfloat c_zrot1[2];
			GLfloat c_up_x1[2];
			GLfloat c_up_y1[2];
			GLfloat c_up_z1[2];

			glm::vec4 light_status1[2];

			GLfloat helper[2][612];

			fs >> camera_num1[0];

			fs >> c_xpos1[0];
			fs >> c_ypos1[0];
			fs >> c_zpos1[0];
			fs >> c_xrot1[0];
			fs >> c_yrot1[0];
			fs >> c_zrot1[0];
			fs >> c_up_x1[0];
			fs >> c_up_y1[0];
			fs >> c_up_z1[0];

			fs >> light_status1[0][0];
			fs >> light_status1[0][1];
			fs >> light_status1[0][2];
			fs >> light_status1[0][3];

			int idx=0;

			for(itr=nodes.begin();itr != nodes.end(); itr++){

				fs >> helper[0][idx];
				idx++;
				fs >> helper[0][idx];
				idx++;
				fs >> helper[0][idx];
				idx++;

				fs >> helper[0][idx];
				idx++;
				fs >> helper[0][idx];
				idx++;
				fs >> helper[0][idx];
				idx++;

				fs >> helper[0][idx];
				idx++;
				fs >> helper[0][idx];
				idx++;
				fs >> helper[0][idx];
				idx++;

				fs >> helper[0][idx];
				idx++;
				fs >> helper[0][idx];
				idx++;
				fs >> helper[0][idx];
				idx++;
			}


			while(fs>>t){
				count++;

				fs >> camera_num1[1];

				fs >> c_xpos1[1];
				fs >> c_ypos1[1];
				fs >> c_zpos1[1];
				fs >> c_xrot1[1];
				fs >> c_yrot1[1];
				fs >> c_zrot1[1];
				fs >> c_up_x1[1];
				fs >> c_up_y1[1];
				fs >> c_up_z1[1];

				fs >> light_status1[1][0];
				fs >> light_status1[1][1];
				fs >> light_status1[1][2];
				fs >> light_status1[1][3];

				idx=0;

				for(itr=nodes.begin();itr != nodes.end(); itr++){

					fs >> helper[1][idx];
					idx++;
					fs >> helper[1][idx];
					idx++;
					fs >> helper[1][idx];
					idx++;

					fs >> helper[1][idx];
					idx++;
					fs >> helper[1][idx];
					idx++;
					fs >> helper[1][idx];
					idx++;

					fs >> helper[1][idx];
					idx++;
					fs >> helper[1][idx];
					idx++;
					fs >> helper[1][idx];
					idx++;

					fs >> helper[1][idx];
					idx++;
					fs >> helper[1][idx];
					idx++;
					fs >> helper[1][idx];
					idx++;
				}

				camera_num=camera_num1[0];

				light_status = light_status1[0];

				GLfloat diff_c_xpos = (c_xpos1[1] - c_xpos1[0])/10.0;
				GLfloat diff_c_ypos = (c_ypos1[1] - c_ypos1[0])/10.0;
				GLfloat diff_c_zpos = (c_zpos1[1] - c_zpos1[0])/10.0;
				GLfloat diff_c_xrot = (c_xrot1[1] - c_xrot1[0])/10.0;
				GLfloat diff_c_yrot = (c_yrot1[1] - c_yrot1[0])/10.0;
				GLfloat diff_c_zrot = (c_zrot1[1] - c_zrot1[0])/10.0;
				GLfloat diff_c_up_x = (c_up_x1[1] - c_up_x1[0])/10.0;
				GLfloat diff_c_up_y = (c_up_y1[1] - c_up_y1[0])/10.0;
				GLfloat diff_c_up_z = (c_up_z1[1] - c_up_z1[0])/10.0;

				//std::cout<<diff_c_xpos<<" "<<diff_c_ypos<<" "<<diff_c_zpos<<" "<<diff_c_xrot<<" "<<diff_c_yrot<<" "<<diff_c_zrot<<" "<<diff_c_up_x<<" "<<diff_c_up_y<<" "<<diff_c_up_z<<"\n";

				GLfloat diff_helper[612];

				for(idx=0;idx<612;idx++){
					diff_helper[idx] = (helper[1][idx] - helper[0][idx])/10.0;
				}

				//if(fs.eof()){
    				//std::cout << "[EoF reached]\n";
    			//	break;
				//}

				idx=0;

				for(int i=1;i<=10;i++){

					//std::cout<<i<<"\n";
					idx=0;

					GLfloat tx1,ty1,tz1,rx1,ry1,rz1,sx1,sy1,sz1,pre_rot_x1,pre_rot_y1,pre_rot_z1;

					if(mode || mode2){
						c_xpos = c_xpos1[0] + i*diff_c_xpos;
						c_ypos = c_ypos1[0] + i*diff_c_ypos;
						c_zpos = c_zpos1[0] + i*diff_c_zpos;
						c_xrot = c_xrot1[0] + i*diff_c_xrot;
						c_yrot = c_yrot1[0] + i*diff_c_yrot;
						c_zrot = c_zrot1[0] + i*diff_c_zrot;
						c_up_x = c_up_x1[0] + i*diff_c_up_x;
						c_up_y = c_up_y1[0] + i*diff_c_up_y;
						c_up_z = c_up_z1[0] + i*diff_c_up_z;

						//std::cout<<c_xpos<<" "<<c_ypos<<" "<<c_zpos<<" "<<c_xrot<<" "<<c_yrot<<" "<<c_zrot<<" "<<c_up_x<<" "<<c_up_y<<" "<<c_up_z<<"\n";

						for(itr=nodes.begin();itr != nodes.end();itr++){
							tx1 = helper[0][idx] + i*diff_helper[idx];
							idx++;
							ty1 = helper[0][idx] + i*diff_helper[idx];
							idx++;
							tz1 = helper[0][idx] + i*diff_helper[idx];
							idx++;
							rx1 = helper[0][idx] + i*diff_helper[idx];
							idx++;
							ry1 = helper[0][idx] + i*diff_helper[idx];
							idx++;
							rz1 = helper[0][idx] + i*diff_helper[idx];
							idx++;
							sx1 = helper[0][idx] + i*diff_helper[idx];
							idx++;
							sy1 = helper[0][idx] + i*diff_helper[idx];
							idx++;
							sz1 = helper[0][idx] + i*diff_helper[idx];
							idx++;
							pre_rot_x1 = helper[0][idx] + i*diff_helper[idx];
							idx++;
							pre_rot_y1 = helper[0][idx] + i*diff_helper[idx];
							idx++;
							pre_rot_z1 = helper[0][idx] + i*diff_helper[idx];
							idx++;

							itr->second->change_parameters(tx1,ty1,tz1,rx1,ry1,rz1,sx1,sy1,sz1,pre_rot_x1,pre_rot_y1,pre_rot_z1);
						}

						renderGL();

						if(mode2){

							int num_bytes_written = 5;

							//std::string s=std::to_string(num_bytes_written);
							//std::cout<<s<<"\n";
							
							std::string input="a";
							input=input+".tga";
							std::string drwfilename=input;

							int width=1000;
							int height=1000;
										
							unsigned char* ustore = new unsigned char[width*height*3];

							glReadBuffer(GL_FRONT);
							
							glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, ustore);
						
							num_bytes_written = stbi_write_tga( drwfilename.c_str(), width, height, 3, (void*)ustore);

						}

						glfwSwapBuffers(window);

						//emulate sleep
						//long long int local=0;
						//while(local<1e8)
							//local++;

						glfwSetTime(0);
						while(glfwGetTime()<0.1){}
					}
				}

				camera_num1[0]=camera_num1[1];
				c_xpos1[0]=c_xpos1[1];
				c_ypos1[0]=c_ypos1[1];
				c_zpos1[0]=c_zpos1[1];
				c_xrot1[0]=c_xrot1[1];
				c_yrot1[0]=c_yrot1[1];
				c_zrot1[0]=c_zrot1[1];
				c_up_x1[0]=c_up_x1[1];
				c_up_y1[0]=c_up_y1[1];
				c_up_z1[0]=c_up_z1[1];

				light_status1[0]=light_status1[1];

				for(idx=0;idx<612;idx++){
					helper[0][idx] = helper[1][idx];
				}
			}

			if(count==1 && (mode || mode2)){
				c_xpos = c_xpos1[0];
				c_ypos = c_ypos1[0];
				c_zpos = c_zpos1[0];
				c_xrot = c_xrot1[0];
				c_yrot = c_yrot1[0];
				c_zrot = c_zrot1[0];
				c_up_x = c_up_x1[0];
				c_up_y = c_up_y1[0];
				c_up_z = c_up_z1[0];

				idx=0;

				//std::cout<<c_xpos<<" "<<c_ypos<<" "<<c_zpos<<" "<<c_xrot<<" "<<c_yrot<<" "<<c_zrot<<" "<<c_up_x<<" "<<c_up_y<<" "<<c_up_z<<"\n";

				for(itr=nodes.begin();itr != nodes.end();itr++){

					GLfloat tx1,ty1,tz1,rx1,ry1,rz1,sx1,sy1,sz1,pre_rot_x1,pre_rot_y1,pre_rot_z1;

					tx1 = helper[0][idx];
					idx++;
					ty1 = helper[0][idx];
					idx++;
					tz1 = helper[0][idx];
					idx++;
					rx1 = helper[0][idx];
					idx++;
					ry1 = helper[0][idx];
					idx++;
					rz1 = helper[0][idx];
					idx++;
					sx1 = helper[0][idx];
					idx++;
					sy1 = helper[0][idx];
					idx++;
					sz1 = helper[0][idx];
					idx++;
					pre_rot_x1 = helper[0][idx];
					idx++;
					pre_rot_y1 = helper[0][idx];
					idx++;
					pre_rot_z1 = helper[0][idx];
					idx++;

					itr->second->change_parameters(tx1,ty1,tz1,rx1,ry1,rz1,sx1,sy1,sz1,pre_rot_x1,pre_rot_y1,pre_rot_z1);
				}
			}

			fs.close();
			std::cout << "Keyframes counted : " << count << "\n";
			mode=false;
			mode1=false;
			mode2=false;
		}
	}

	glfwTerminate();
	return 0;
}
