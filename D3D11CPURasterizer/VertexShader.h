#pragma once
#include <glm/glm.hpp>

struct VSInput {
	glm::vec3 pos;
	glm::vec3 color;
};

struct VSOutput {
	glm::vec3 pos;
	glm::vec3 color;
};

VSOutput vs_main(VSInput &input) {
	VSOutput output;
	output.pos = input.pos;
	output.color = input.color;
	return output;
}