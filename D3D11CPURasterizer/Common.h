#pragma once
#include <glm/glm.hpp>

struct Light {
	float strength;
	glm::vec3 pos;
};

struct Mat {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

inline struct Constants {
	glm::mat4 model;
	glm::mat4 invTr;
	Light light;
	Mat mat;
} constants;