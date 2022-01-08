#include "gl_framework.hpp"
#include "hnode.hpp"
#include "camera.hpp"
#include <map>
#include <fstream>

extern GLfloat c_xrot, c_yrot, c_zrot;
extern GLfloat c_xpos, c_ypos, c_zpos;
extern GLfloat c_up_x, c_up_y, c_up_z;
extern GLfloat zoom;
extern glm::vec4 light_status;
extern bool enable_perspective, mode, mode1, mode2;
extern csX75::HNode* curr_node;
extern std::map<std::string, csX75::HNode*> nodes;
extern int camera_num;

int time_stamp = 0;

namespace csX75
{
	//! Initialize GL State
	void initGL(void)
	{
		// Set framebuffer clear color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// Set depth buffer furthest depth
		glClearDepth(1.0);
		// Set depth test to less-than
		glDepthFunc(GL_LESS);
		// Enable depth testing
		glEnable(GL_DEPTH_TEST);
	}
	
	//! GLFW Error Callback
	void error_callback(int error, const char* description)
	{
		std::cerr << description << std::endl;
	}
	
	//! GLFW framebuffer resize callback
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// Resize the viewport to fit the window size - draw to entire window
		glViewport(0, 0, width, height);
	}
	
	//!GLFW keyboard callback
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		// Close the window if the ESC key was pressed
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

		// Choosing the nodes, choosing root selects all 3 
		else if(key == GLFW_KEY_U && action == GLFW_PRESS)
			curr_node = nodes["root"];

		// Choosing nodes for the rider
		else if(key == GLFW_KEY_0 && action == GLFW_PRESS)
			curr_node = nodes["hip"];
		else if(key == GLFW_KEY_T && action == GLFW_PRESS)
			curr_node = nodes["torso"];
		else if(key == GLFW_KEY_N && action == GLFW_PRESS)
			curr_node = nodes["neck"];
		else if(key == GLFW_KEY_1 && action == GLFW_PRESS)
			curr_node = nodes["head"];
		else if(key == GLFW_KEY_2 && action == GLFW_PRESS)
			curr_node = nodes["lupper_arm"]; 
		else if(key == GLFW_KEY_3 && action == GLFW_PRESS)
			curr_node = nodes["rupper_arm"]; 
		else if(key == GLFW_KEY_4 && action == GLFW_PRESS)
			curr_node = nodes["llower_arm"]; 
		else if(key == GLFW_KEY_5 && action == GLFW_PRESS)
			curr_node = nodes["rlower_arm"]; 
		else if(key == GLFW_KEY_6 && action == GLFW_PRESS)
			curr_node = nodes["lupper_leg"]; 
		else if(key == GLFW_KEY_7 && action == GLFW_PRESS)
			curr_node = nodes["rupper_leg"]; 
		else if(key == GLFW_KEY_8 && action == GLFW_PRESS)
			curr_node = nodes["llower_leg"]; 
		else if(key == GLFW_KEY_9 && action == GLFW_PRESS)
			curr_node = nodes["rlower_leg"]; 

		// Choosing the node for the bike
		else if(key == GLFW_KEY_G && action == GLFW_PRESS)
			curr_node = nodes["engine"];
		else if(key == GLFW_KEY_H && action == GLFW_PRESS)
			curr_node = nodes["back_tire"];
		else if(key == GLFW_KEY_I && action == GLFW_PRESS)
			curr_node = nodes["front_tire"]; 
		else if(key == GLFW_KEY_J && action == GLFW_PRESS)
			curr_node = nodes["handlebar"]; 

		// Choosing rotation keys
		else if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){	
			if(curr_node == nodes["front_tire"] || curr_node == nodes["back_tire"] || curr_node == nodes["rlower_leg"]
					|| curr_node == nodes["llower_arm"] || curr_node == nodes["rlower_arm"] || curr_node == nodes["llower_leg"])
				; // Empty if
			else 
				curr_node->dec_ry();
		}	
		else if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS){	
			if(curr_node == nodes["front_tire"] || curr_node == nodes["back_tire"] || curr_node == nodes["rlower_leg"]
					|| curr_node == nodes["llower_arm"] || curr_node == nodes["rlower_arm"] || curr_node == nodes["llower_leg"])
				; // Empty if
			else 
				curr_node->inc_ry();
		}
		else if(key == GLFW_KEY_UP && action == GLFW_PRESS){
			if(curr_node == nodes["front_tire"] || curr_node == nodes["back_tire"] || curr_node == nodes["handlebar"]
					|| curr_node == nodes["llower_arm"] || curr_node == nodes["rlower_arm"])
				; // Empty if
			else curr_node->dec_rx();
		}
		else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
			if(curr_node == nodes["front_tire"] || curr_node == nodes["back_tire"] || curr_node == nodes["handlebar"]
					|| curr_node == nodes["llower_arm"] || curr_node == nodes["rlower_arm"])
				; // Empty ifS
			else 
				curr_node->inc_rx();
		}
		else if(key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS){
			if(curr_node == nodes["llower_leg"] || curr_node == nodes["rlower_leg"])
				; // Empty if
			else
				curr_node->dec_rz();
		}
		else if(key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS){
			if(curr_node == nodes["llower_leg"] || curr_node == nodes["rlower_leg"])
				; // Empty if
			else
				curr_node->inc_rz();
		}

		// Choosing global translation keys, allow translation for all 3 together, allow translation for rider and bike as a whole
		else if(key == GLFW_KEY_A && action == GLFW_PRESS && 
				(curr_node == nodes["root"] || curr_node == nodes["hip"] || curr_node == nodes["engine"]))
			curr_node->dec_tx();
		else if(key == GLFW_KEY_B && action == GLFW_PRESS && 
				(curr_node == nodes["root"] || curr_node == nodes["hip"] || curr_node == nodes["engine"]))
			curr_node->inc_tx();
		else if(key == GLFW_KEY_C && action == GLFW_PRESS && 
				(curr_node == nodes["root"] || curr_node == nodes["hip"] || curr_node == nodes["engine"]))
			curr_node->dec_ty();
		else if(key == GLFW_KEY_D && action == GLFW_PRESS && 
				(curr_node == nodes["root"] || curr_node == nodes["hip"] || curr_node == nodes["engine"]))
			curr_node->inc_ty();
		else if(key == GLFW_KEY_E && action == GLFW_PRESS && 
				(curr_node == nodes["root"] || curr_node == nodes["hip"] || curr_node == nodes["engine"]))
			curr_node->dec_tz();
		else if(key == GLFW_KEY_F && action == GLFW_PRESS && 
				(curr_node == nodes["root"] || curr_node == nodes["hip"] || curr_node == nodes["engine"]))
			curr_node->inc_tz();

		// Camera Controls
		else if((key == GLFW_KEY_KP_ADD || (key == GLFW_KEY_EQUAL && mods == GLFW_MOD_SHIFT)) && action == GLFW_PRESS){
			zoom *= 1.1; // Hard coded factor for now
		}
		else if((key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS) && action == GLFW_PRESS){
			zoom *= (1 / 1.1);
		}

		else if(key == GLFW_KEY_Z && action == GLFW_PRESS){
			camera_num = (camera_num + 1) % 3;
		}

		// Light Controls
		else if(key == GLFW_KEY_K && action == GLFW_PRESS){
			light_status[0] = 1.0 - light_status[0];
		}
		else if(key == GLFW_KEY_M && action == GLFW_PRESS){
			light_status[1] = 1.0 - light_status[1];
		}
		else if(key == GLFW_KEY_O && action == GLFW_PRESS){
			light_status[2] = 1.0 - light_status[2];
		}
		else if(key == GLFW_KEY_Q && action == GLFW_PRESS){
			light_status[3] = 1.0 - light_status[3];
		}

		// Saving frames
		else if(key == GLFW_KEY_S && action == GLFW_PRESS){
			std::ofstream log;
			log.open("keyframes.txt", std::ofstream::app);

			log << time_stamp << " ";
			time_stamp += 10;

			log << camera_num << " ";
			log << c_xpos << " " << c_ypos << " " << c_zpos << " ";
			log << c_xrot << " " << c_yrot << " " << c_zrot << " ";
			log << c_up_x << " " << c_up_y << " " << c_up_z << " ";

			for(int i = 0; i < 4; i++)
				log << light_status[i] << " ";

			std::map<std::string, csX75::HNode*>::iterator itr = nodes.begin();

			for(itr; itr != nodes.end(); itr++){
				log << itr->second->tx << " " << itr->second->ty << " " << itr->second->tz << " ";
				log << itr->second->rx << " " << itr->second->ry << " " << itr->second->rz << " ";
				log << itr->second->sx << " " << itr->second->sy << " " << itr->second->sz << " ";
				log << itr->second->pre_rot_x << " " << itr->second->pre_rot_y << " " << itr->second->pre_rot_z << " ";
			}
			log << "\n";

			log.close();
		}

		else if(key == GLFW_KEY_P && action == GLFW_PRESS){
			mode = true;
		}
		else if(key == GLFW_KEY_L && action == GLFW_PRESS){
			mode1 = true;
		}
		else if(key == GLFW_KEY_R && action == GLFW_PRESS){
			mode2 = true;
		}
	}
};  
	


