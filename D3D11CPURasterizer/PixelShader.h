#pragma once
#include <glm/glm.hpp>
#include "Common.h"

struct PSInput {
	glm::vec3 pos_world;
	glm::vec3 color;
	glm::vec2 uv;
	glm::vec3 normal;
};

inline glm::vec3 blinn_phong(const glm::vec3 &normal, const glm::vec3 &light_vec, const glm::vec3 &cam_dir) {
	const auto halfway = glm::normalize(light_vec + cam_dir);

	const auto diffuse = glm::max(glm::dot(normal, light_vec), 0.0f) * constants.mat.diffuse;
	const auto specular = glm::pow(glm::max(glm::dot(halfway, normal), 0.0f), constants.mat.shininess) * constants.mat.specular;

	return constants.mat.ambient + (diffuse + specular) * constants.light.strength;
}

inline glm::vec4 ps_color_main(PSInput &input) {
	return glm::vec4(input.color, 1.0f);
}

inline glm::vec4 ps_uv_main(PSInput &input) {
	float size = 10.0f;
	auto pos = glm::floor(input.uv * size);
	float pattern = glm::mod(pos.x + pos.y, 2.0f);
	return glm::vec4(glm::vec3(pattern), 1.0f);
}

inline glm::vec4 ps_blinn_phong_main(PSInput &input) {
	const auto light_vec = glm::normalize(constants.light.pos - input.pos_world);
	const auto cam_pos = glm::vec3(0.0f, 0.0f, -1.0f);
	const auto cam_vec = glm::normalize(cam_pos - input.pos_world);
	const auto color = blinn_phong(input.normal, light_vec, cam_vec);
	return glm::vec4(color, 1.0f);
}