#pragma once
#include <glm/glm.hpp>
#include "Vertex.h"

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<int> indices;

	glm::vec3 ambient = glm::vec3(0.1f);
	glm::vec3 diffuse = glm::vec3(0.7f);
	glm::vec3 specular = glm::vec3(0.5f);
	float shininess = 32.0f;
};
