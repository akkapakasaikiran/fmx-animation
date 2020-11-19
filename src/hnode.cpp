#include "hnode.hpp"

#include <iostream>

extern GLuint vPosition,vColor,vNormal,uModelViewMatrix, viewMatrix, normalMatrix;
extern std::vector<glm::mat4> matrixStack;
extern glm::mat3 normal_matrix;
extern glm::mat4 view_matrix;

namespace csX75
{

	HNode::HNode(HNode* a_parent, GLuint num_v, glm::vec4* posns_arr, std::size_t v_size, glm::vec4 col, glm::vec4* normals_arr){
		num_vertices = num_v;
		vertex_buffer_size = v_size;
		color = col;

		// Construct an array of colors
		glm::vec4 colors_arr[num_vertices];
		for(int i = 0; i < num_vertices; i++)
			colors_arr[i] = color;

		// Initialize vao and vbo of the object
		// Ask GL for a Vertex Array Object (vao)
		glGenVertexArrays(1, &vao);
		// Ask GL for aVertex Buffer Object (vbo)
		glGenBuffers(1, &vbo);

		// Bind them to the current context
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Populate the VBO using arrays
		glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size + vertex_buffer_size, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_buffer_size, posns_arr);
		glBufferSubData(GL_ARRAY_BUFFER, vertex_buffer_size, vertex_buffer_size, colors_arr);
		glBufferSubData(GL_ARRAY_BUFFER, 2*vertex_buffer_size, vertex_buffer_size, normals_arr);

		// Setup the vertex array as per the shader
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertex_buffer_size));

		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*vertex_buffer_size));

		// Set parent
		if(a_parent == NULL)
			parent = NULL;
		else{
			parent = a_parent;
			parent->add_child(this);
		}

		// Initialize parameters 
		tx = ty = tz = 0;
		rx = ry = rz = 0;
		sx = sy = sz = 0.5;

		update_matrices();
	}

	void HNode::update_matrices(){
		scaling = glm::scale(glm::mat4(1.0f), glm::vec3(sx,sy,sz));

		rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rx), glm::vec3(1.0f,0.0f,0.0f));
		rotation = glm::rotate(rotation, glm::radians(ry), glm::vec3(0.0f,1.0f,0.0f));
		rotation = glm::rotate(rotation, glm::radians(rz), glm::vec3(0.0f,0.0f,1.0f));

		translation = glm::translate(glm::mat4(1.0f), glm::vec3(tx,ty,tz));

		pre_rotation_translation = glm::translate(glm::mat4(1.0f), glm::vec3(pre_rot_x,pre_rot_y,pre_rot_z));
	}

	void HNode::add_child(HNode* a_child){
		children.push_back(a_child);
	}

	void HNode::change_parameters(GLfloat atx, GLfloat aty, GLfloat atz,
									GLfloat arx, GLfloat ary, GLfloat arz,
									GLfloat asx, GLfloat asy, GLfloat asz,
									GLfloat apre_rot_x, GLfloat apre_rot_y, GLfloat apre_rot_z){
		tx = atx; ty = aty; tz = atz;
		rx = arx; ry = ary; rz = arz;
		sx = asx; sy = asy; sz = asz;
		pre_rot_x = apre_rot_x;
		pre_rot_y = apre_rot_y;
		pre_rot_z = apre_rot_z;

		update_matrices();
	}


	void HNode::render(){
		glm::mat4* ms_mult = multiply_stack(matrixStack);
		*ms_mult = (*ms_mult) * scaling;

		glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(*ms_mult));
		normal_matrix = glm::transpose (glm::inverse(glm::mat3(*ms_mult)));
		glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
		glm::vec3 v = glm::vec3(0.0, 0.0, 1.0);
		v = normal_matrix * v;
		std::cout<<v.x<<" "<<v.y<<" "<<v.z<<"\n";
		glm::vec3 n = glm::normalize(glm::vec3(v));
		glm::vec4 lightPos = glm::vec4(10.0, 20.0, 30.0, 0.0);
  		glm::vec3 lightDir = glm::vec3(view_matrix * lightPos);  // Transforms with camera
  		lightDir = glm::normalize( glm::vec3(lightDir));
  		float dotProduct = glm::dot(n, lightDir);
  		std::cout<<dotProduct<<"\n";
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, num_vertices);
		// delete ms_mult;
	}

	void HNode::render_tree(){
		matrixStack.push_back(translation);
		matrixStack.push_back(rotation);
		matrixStack.push_back(pre_rotation_translation);

		render();
		for(int i=0;i<children.size();i++)
			children[i]->render_tree();

		matrixStack.pop_back();
		matrixStack.pop_back();
		matrixStack.pop_back();
	}

	void HNode::inc_rx(){
		rx += 4; update_matrices();
	}

	void HNode::inc_ry(){
		ry += 4; update_matrices();
	}

	void HNode::inc_rz(){
		rz += 4; update_matrices();
	}

	void HNode::dec_rx(){
		rx -= 4; update_matrices();
	}

	void HNode::dec_ry(){
		ry -= 4; update_matrices();
	}

	void HNode::dec_rz(){
		rz -= 4; update_matrices();
	}

	void HNode::inc_tx(){
		tx += 0.2; update_matrices();
	}

	void HNode::inc_ty(){
		ty += 0.2; update_matrices();
	}

	void HNode::inc_tz(){
		tz += 0.2; update_matrices();
	}

	void HNode::dec_tx(){
		tx -= 0.2; update_matrices();
	}

	void HNode::dec_ty(){
		ty -= 0.2; update_matrices();
	}

	void HNode::dec_tz(){
		tz -= 0.2; update_matrices();
	}


	glm::mat4* multiply_stack(std::vector<glm::mat4> matStack){
		glm::mat4* mult;
		mult = new glm::mat4(1.0f);
	
		for(int i = 0; i < matStack.size(); i++){
			*mult = (*mult) * matStack[i];
		}	

		return mult;
	}

	glm::mat4 HNode::get_transformation(){
		return translation * rotation * pre_rotation_translation;
	}

}