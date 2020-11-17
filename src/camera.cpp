#include "camera.hpp"

namespace csX75 {

	Camera::Camera(glm::vec3 posn, glm::vec3 rot, glm::vec3 upv, glm::vec3 lookat_ptv){
		position = posn;
		rotation = rot;
		up = upv;
		lookat_pt = lookat_ptv;
	}

};