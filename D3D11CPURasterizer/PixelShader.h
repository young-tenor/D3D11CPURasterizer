#pragma once
#include <glm/glm.hpp>

struct PSInput {
	glm::vec3 color;
};

inline glm::vec4 ps_main(PSInput &input) {
	return glm::vec4(input.color, 1.0f);
}