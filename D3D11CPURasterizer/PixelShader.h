#pragma once
#include <glm/glm.hpp>
#include "Common.h"

struct PSInput {
	glm::vec3 posWolrd;
	glm::vec3 color;
	glm::vec2 uv;
	glm::vec3 normal;
};

inline glm::vec3 ShadeBlinnPhong(const glm::vec3 &normal, const glm::vec3 &lightVec, const glm::vec3 &toEye) {
	const auto halfway = glm::normalize(lightVec + toEye);

	const auto diffuse = glm::max(glm::dot(normal, lightVec), 0.0f) * constants.mat.diffuse;
	const auto specular = glm::pow(glm::max(glm::dot(halfway, normal), 0.0f), constants.mat.shininess) * constants.mat.specular;

	return constants.mat.ambient + (diffuse + specular) * constants.light.strength;
}

inline glm::vec4 PSColorMain(PSInput &input) {
	return glm::vec4(input.color, 1.0f);
}

inline glm::vec4 PSUVMain(PSInput &input) {
	float size = 10.0f;
	auto pos = glm::floor(input.uv * size);
	float pattern = glm::mod(pos.x + pos.y, 2.0f);
	return glm::vec4(glm::vec3(pattern), 1.0f);
}

inline glm::vec4 PSBlinnPhongMain(PSInput &input) {
	const auto lightVec = glm::normalize(constants.light.pos - input.posWolrd);
	const auto camPos = glm::vec3(0.0f, 0.0f, -1.0f);
	const auto toEye = glm::normalize(camPos - input.posWolrd);
	const auto color = ShadeBlinnPhong(input.normal, lightVec, toEye);
	return glm::vec4(color, 1.0f);
}