#ifndef _COMMON_HPP_
#define _COMMON_HPP_

// Defining the ESCAPE Key Code
#define ESCAPE 27
// Defining the DELETE Key Code
#define DELETE 127

#include <vector>
#include <map>

#include "gl_framework.hpp"
#include "shader_util.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/constants.hpp" 
#include "glm/trigonometric.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/string_cast.hpp>

#include "hnode.hpp"
#include "camera.hpp"

// Camera position and rotation Parameters
//GLfloat c_xpos = -50.0, c_ypos = 30.0, c_zpos = 40.0;
GLfloat c_xpos = 0.0, c_ypos = 0.0, c_zpos = 20.0;
GLfloat c_up_x = 0.0, c_up_y = 1.0, c_up_z = 0.0;
GLfloat c_xrot = 0.0, c_yrot = 0.0, c_zrot = 0.0;


// Running variable to toggle culling on/off
bool enable_culling = true;
// Running variable to toggle wireframe/solid modelling
bool solid = true;
// Shader program attribs
GLuint vPosition, vColor, vNormal;

//global matrix stack for hierarchical modelling
std::vector<glm::mat4> matrixStack;

csX75::HNode* root_node;
csX75::HNode* curr_node;
std::map<std::string, csX75::HNode*> nodes;

int camera_num;
csX75::Camera* curr_camera;
std::map<std::string, csX75::Camera*> cameras;

//-------------------------------------------------------------------------

#endif
