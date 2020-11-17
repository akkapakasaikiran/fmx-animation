#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace csX75 {

	class Camera{
	public:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 up;
		glm::vec3 lookat_pt;
	
		Camera(glm::vec3, glm::vec3, glm::vec3, glm::vec3);

	};

};

#endif