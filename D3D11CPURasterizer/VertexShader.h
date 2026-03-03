#pragma once
#include <glm/glm.hpp>

inline struct Constants {
	glm::mat4 model;
} constants;

struct VSInput {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 uv;
};

struct VSOutput {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 uv;
};

inline VSOutput vs_main(VSInput &input) {
	VSOutput output;
	output.pos = constants.model * glm::vec4(input.pos, 1.0f);
	output.color = input.color;
	output.uv = input.uv;
	return output;
}