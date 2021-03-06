#ifndef _HNODE_HPP_
#define _HNODE_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "gl_framework.hpp"

namespace csX75	 { 

	// A simple class that represents a node in the hierarchy tree
	class HNode {
		GLuint num_vertices;
		GLuint vao, vbo;

		bool using_texture;
		uint texture;

		std::string name;
		glm::vec4 color;

		glm::mat4 rotation;
		glm::mat4 translation;
		glm::mat4 pre_rotation_translation;
		
		std::vector<HNode*> children;
		HNode* parent;

		void update_matrices();

	public:

		GLfloat tx,ty,tz,rx,ry,rz,sx,sy,sz;

		GLfloat pre_rot_x,pre_rot_y,pre_rot_z;

		glm::mat4 scaling;

		HNode (HNode*, GLuint, glm::vec4*, std::size_t, glm::vec4, glm::vec4*, std::string = "", glm::vec2* = NULL);
		HNode (HNode*, GLuint, glm::vec4*, std::size_t, glm::vec4);

		void add_child(HNode*);
		void render();
		void change_parameters(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
							GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
		void render_tree();
		void inc_rx(); void inc_ry(); void inc_rz();
		void dec_rx(); void dec_ry(); void dec_rz();
		void inc_tx(); void inc_ty(); void inc_tz();
		void dec_tx(); void dec_ty(); void dec_tz();
		glm::mat4 get_transformation();
		glm::mat4 get_scaling();
	};

	glm::mat4* multiply_stack(std::vector<glm::mat4>);
};	

#endif