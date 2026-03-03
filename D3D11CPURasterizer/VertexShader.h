#pragma once
#include <glm/glm.hpp>
#include "Common.h"

struct VSInput {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct VSOutput {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 uv;
	glm::vec3 normal;
};

inline VSOutput vs_main(VSInput &input) {
	VSOutput output;
	output.pos = constants.model * glm::vec4(input.pos, 1.0f);
	output.color = input.color;
	output.uv = input.uv;
	output.normal = constants.invTr * glm::vec4(input.normal, 0.0f);
	return output;
}