#include "gl_framework.hpp"
#include "hnode.hpp"
#include <map>

extern GLfloat c_xrot,c_yrot,c_zrot,c_xpos,c_ypos,c_zpos;
extern bool enable_perspective;
extern csX75::HNode *curr_node, *root_node;
extern std::map<std::string, csX75::HNode*> nodes;

namespace csX75
{
	//! Initialize GL State
	void initGL(void)
	{
		//Set framebuffer clear color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//Set depth buffer furthest depth
		glClearDepth(1.0);
		//Set depth test to less-than
		glDepthFunc(GL_LESS);
		//Enable depth testing
		glEnable(GL_DEPTH_TEST);
	}
	
	//!GLFW Error Callback
	void error_callback(int error, const char* description)
	{
		std::cerr<<description<<std::endl;
	}
	
	//!GLFW framebuffer resize callback
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		//!Resize the viewport to fit the window size - draw to entire window
		glViewport(0, 0, width, height);
	}
	
	//!GLFW keyboard callback
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		//!Close the window if the ESC key was pressed
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

		// Choosing the node
		else if(key == GLFW_KEY_0 && action == GLFW_PRESS)
			curr_node = nodes["engine"];
		else if(key == GLFW_KEY_1 && action == GLFW_PRESS)
			curr_node = nodes["back_tire"];
		else if(key == GLFW_KEY_2 && action == GLFW_PRESS)
			curr_node = nodes["front_tire"]; 
		else if(key == GLFW_KEY_3 && action == GLFW_PRESS)
			curr_node = nodes["handlebar"]; 
 
 		// Rotations with relevant constrants
		else if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){
			if(curr_node == nodes["handlebar"] || curr_node == root_node)
				curr_node->dec_ry();
		}
		else if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
			if(curr_node == nodes["handlebar"] || curr_node == root_node)
				curr_node->inc_ry();
		}
		else if(key == GLFW_KEY_UP && action == GLFW_PRESS && curr_node == root_node){
			curr_node->dec_rx();
		}
		else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS && curr_node == root_node){
			curr_node->inc_rx();
		}
		else if(key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS){
			curr_node->dec_rz();
		}
		else if(key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS){
			curr_node->inc_rz();
		}

		// Global translation, only for the root node, in this case, engine
		else if(key == GLFW_KEY_A && action == GLFW_PRESS && curr_node == nodes["engine"])
			curr_node->dec_tx();
		else if(key == GLFW_KEY_B && action == GLFW_PRESS && curr_node == nodes["engine"])
			curr_node->inc_tx();
		else if(key == GLFW_KEY_C && action == GLFW_PRESS && curr_node == nodes["engine"])
			curr_node->dec_ty();
		else if(key == GLFW_KEY_D && action == GLFW_PRESS && curr_node == nodes["engine"])
			curr_node->inc_ty();
		else if(key == GLFW_KEY_E && action == GLFW_PRESS && curr_node == nodes["engine"])
			curr_node->dec_tz();
		else if(key == GLFW_KEY_F && action == GLFW_PRESS && curr_node == nodes["engine"])
			curr_node->inc_tz();
		// Camera Controls
		else if((key == GLFW_KEY_KP_ADD || (key == GLFW_KEY_EQUAL && mods==GLFW_MOD_SHIFT)) && action == GLFW_PRESS){
			c_xpos+=1;
			c_ypos-=0.6;
			c_ypos-=0.8;
		}
		else if((key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS) && action == GLFW_PRESS){
			c_xpos-=1;
			c_ypos+=0.6;
			c_ypos+=0.8;
		}  
	}
};  
	


