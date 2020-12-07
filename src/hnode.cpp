#include "hnode.hpp"
#include <iostream>
#include<bits/stdc++.h>

// #define STB_IMAGE_IMPLEMENTATION
// #define STBI_ONLY_TGA
// #include "stb_image_read.hpp"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.hpp"


extern GLuint vPosition, vColor, vNormal, vTexCoord;
extern GLuint MVP, ModelviewMatrix, normalMatrix, light_stat;
extern std::vector<glm::mat4> matrixStack, matrixStack1;
extern glm::vec4 light_status;
extern glm::mat3 normal_matrix;
extern glm::mat4 view_matrix;

namespace csX75
{

	HNode::HNode(HNode* a_parent, GLuint num_v, glm::vec4* posns_arr, std::size_t v_size, glm::vec4 col, 
		glm::vec4* normals_arr, std::string texfilepath, glm::vec2* texcoord_arr)
	{
		num_vertices = num_v;
		color = col;
		using_texture = (texfilepath != "");

		// Construct an array of colors
		glm::vec4 colors_arr[num_vertices];
		for(int i = 0; i < num_vertices; i++)
			colors_arr[i] = color;

		// Assign false values if node is not being texture mappes
		// Check this in fshader to figure out color
		if(texcoord_arr == NULL){
			texcoord_arr = (glm::vec2*) malloc(num_v * sizeof(glm::vec2));
			for(int i = 0; i < num_v; i++)
				texcoord_arr[i] = glm::vec2(-1.0, -1.0);
		}

		// Initialize vao and vbo of the object
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		// Bind them to the current context
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Populate the VBO using arrays
		glBufferData(GL_ARRAY_BUFFER, (1+1+1+0.5)*v_size, NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0*v_size, v_size, posns_arr);
		glBufferSubData(GL_ARRAY_BUFFER, 1*v_size, v_size, colors_arr);
		glBufferSubData(GL_ARRAY_BUFFER, 2*v_size, v_size, normals_arr);
		glBufferSubData(GL_ARRAY_BUFFER, 3*v_size, 0.5*v_size, texcoord_arr); // 0.5*v_size because vec2 not vec4

		// Setup the vertex array as per the shader
		glEnableVertexAttribArray(vPosition);
		glEnableVertexAttribArray(vColor);
		glEnableVertexAttribArray(vNormal);
		glEnableVertexAttribArray(vTexCoord);

		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(v_size));
		glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*v_size));
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(3*v_size));

		// load and create a texture 
		if(using_texture){
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			unsigned char header[54];// 54 Byte header of BMP
			int pos;
			uint w, h, size; // size = w*h*3
			unsigned char* data; // Data in RGB FORMAT
			FILE* file = fopen(texfilepath.c_str(), "rb"); 
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
			if(size == 0) size = w*h*3;
			if(pos == 0) pos = 54;

			data = new unsigned char[size];

			fread(data, size, 1, file); // read the file
			fclose(file);

			if(data){
				// std::cout << w << " " << h << std::endl;
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
				std::cout << "Failed to load texture" << std::endl;

			free(data);	
		}

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

		glm::mat4* ms_mult1 = multiply_stack(matrixStack1);
		*ms_mult1 = (*ms_mult1) * scaling;

		glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(*ms_mult));
		normal_matrix = glm::transpose (glm::inverse(glm::mat3(*ms_mult1)));
		glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
		glUniformMatrix4fv(ModelviewMatrix, 1, GL_FALSE, glm::value_ptr(*ms_mult1));
		glUniform4fv(light_stat, 1, glm::value_ptr(light_status));

		// bind Texture
		if(using_texture)
        	glBindTexture(GL_TEXTURE_2D, texture);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, num_vertices);
	}

	void HNode::render_tree(){
		matrixStack.push_back(translation);
		matrixStack.push_back(rotation);
		matrixStack.push_back(pre_rotation_translation);

		matrixStack1.push_back(translation);
		matrixStack1.push_back(rotation);
		matrixStack1.push_back(pre_rotation_translation);

		render();
		for(int i=0;i<children.size();i++)
			children[i]->render_tree();

		matrixStack.pop_back();
		matrixStack.pop_back();
		matrixStack.pop_back();

		matrixStack1.pop_back();
		matrixStack1.pop_back();
		matrixStack1.pop_back();
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
		tx += 0.05; update_matrices();
	}

	void HNode::inc_ty(){
		ty += 0.05; update_matrices();
	}

	void HNode::inc_tz(){
		tz += 0.05; update_matrices();
	}

	void HNode::dec_tx(){
		tx -= 0.05; update_matrices();
	}

	void HNode::dec_ty(){
		ty -= 0.05; update_matrices();
	}

	void HNode::dec_tz(){
		tz -= 0.05; update_matrices();
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