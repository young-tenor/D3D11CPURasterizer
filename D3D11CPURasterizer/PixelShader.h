#pragma once
#include <glm/glm.hpp>

struct PSInput {
	glm::vec3 color;
	glm::vec2 uv;
};

inline glm::vec4 ps_color_main(PSInput &input) {
	return glm::vec4(input.color, 1.0f);
}

inline glm::vec4 ps_uv_main(PSInput &input) {
	float size = 10.0f;
	auto pos = glm::floor(input.uv * size);
	float pattern = glm::mod(pos.x + pos.y, 2.0f);
	return glm::vec4(glm::vec3(pattern), 1.0f);
}