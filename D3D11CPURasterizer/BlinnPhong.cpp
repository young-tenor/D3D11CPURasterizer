#include "pch.h"
#include "BlinnPhong.h"
#include "Vertex.h"
#include "VertexShader.h"
#include "PixelShader.h"

bool BlinnPhong::init(HWND h_wnd) {
	if (!App::init(h_wnd)) {
		return false;
	}

	perspective = true;
	perspective_correction = true;
	depth_buffering = true;

	pixel_shader = ps_blinn_phong_main;

	cube = new Mesh();

	// front
	{
		int offset = cube->vertices.size();
		Vertex v0, v1, v2, v3;
		v0.pos = glm::vec3(-0.5f, 0.5f, -0.5f);
		v1.pos = glm::vec3(0.5f, 0.5f, -0.5f);
		v2.pos = glm::vec3(0.5f, -0.5f, -0.5f);
		v3.pos = glm::vec3(-0.5f, -0.5f, -0.5f);
		v0.color = v1.color = v2.color = v3.color = glm::vec3(1.0f);
		v0.normal = v1.normal = v2.normal = v3.normal = glm::vec3(0.0f, 0.0f, -1.0f);
		cube->vertices.push_back(v0);
		cube->vertices.push_back(v1);
		cube->vertices.push_back(v2);
		cube->vertices.push_back(v3);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 1);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 3);
	}
	// back
	{
		int offset = cube->vertices.size();
		Vertex v0, v1, v2, v3;
		v0.pos = glm::vec3(0.5f, 0.5f, 0.5f);
		v1.pos = glm::vec3(-0.5f, 0.5f, 0.5f);
		v2.pos = glm::vec3(-0.5f, -0.5f, 0.5f);
		v3.pos = glm::vec3(0.5f, -0.5f, 0.5f);
		v0.color = v1.color = v2.color = v3.color = glm::vec3(1.0f);
		v0.normal = v1.normal = v2.normal = v3.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		cube->vertices.push_back(v0);
		cube->vertices.push_back(v1);
		cube->vertices.push_back(v2);
		cube->vertices.push_back(v3);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 1);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 3);
	}
	// right
	{
		int offset = cube->vertices.size();
		Vertex v0, v1, v2, v3;
		v0.pos = glm::vec3(0.5f, 0.5f, -0.5f);
		v1.pos = glm::vec3(0.5f, 0.5f, 0.5f);
		v2.pos = glm::vec3(0.5f, -0.5f, 0.5f);
		v3.pos = glm::vec3(0.5f, -0.5f, -0.5f);
		v0.color = v1.color = v2.color = v3.color = glm::vec3(1.0f);
		v0.normal = v1.normal = v2.normal = v3.normal = glm::vec3(1.0f, 0.0f, 0.0f);
		cube->vertices.push_back(v0);
		cube->vertices.push_back(v1);
		cube->vertices.push_back(v2);
		cube->vertices.push_back(v3);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 1);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 3);
	}
	// left
	{
		int offset = cube->vertices.size();
		Vertex v0, v1, v2, v3;
		v0.pos = glm::vec3(-0.5f, 0.5f, 0.5f);
		v1.pos = glm::vec3(-0.5f, 0.5f, -0.5f);
		v2.pos = glm::vec3(-0.5f, -0.5f, -0.5f);
		v3.pos = glm::vec3(-0.5f, -0.5f, 0.5f);
		v0.color = v1.color = v2.color = v3.color = glm::vec3(1.0f);
		v0.normal = v1.normal = v2.normal = v3.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
		cube->vertices.push_back(v0);
		cube->vertices.push_back(v1);
		cube->vertices.push_back(v2);
		cube->vertices.push_back(v3);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 1);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 3);
	}
	// top
	{
		int offset = cube->vertices.size();
		Vertex v0, v1, v2, v3;
		v0.pos = glm::vec3(-0.5f, 0.5f, 0.5f);
		v1.pos = glm::vec3(0.5f, 0.5f, 0.5f);
		v2.pos = glm::vec3(0.5f, 0.5f, -0.5f);
		v3.pos = glm::vec3(-0.5f, 0.5f, -0.5f);
		v0.color = v1.color = v2.color = v3.color = glm::vec3(1.0f);
		v0.normal = v1.normal = v2.normal = v3.normal = glm::vec3(0.0f, 1.0f, 0.0f);
		cube->vertices.push_back(v0);
		cube->vertices.push_back(v1);
		cube->vertices.push_back(v2);
		cube->vertices.push_back(v3);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 1);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 3);
	}
	// bottom
	{
		int offset = cube->vertices.size();
		Vertex v0, v1, v2, v3;
		v0.pos = glm::vec3(-0.5f, -0.5f, -0.5f);
		v1.pos = glm::vec3(0.5f, -0.5f, -0.5f);
		v2.pos = glm::vec3(0.5f, -0.5f, 0.5f);
		v3.pos = glm::vec3(-0.5f, -0.5f, 0.5f);
		v0.color = v1.color = v2.color = v3.color = glm::vec3(1.0f);
		v0.normal = v1.normal = v2.normal = v3.normal = glm::vec3(0.0f, -1.0f, 0.0f);
		cube->vertices.push_back(v0);
		cube->vertices.push_back(v1);
		cube->vertices.push_back(v2);
		cube->vertices.push_back(v3);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 1);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 0);
		cube->indices.push_back(offset + 2);
		cube->indices.push_back(offset + 3);
	}

	meshes.push_back(cube);

	constants.mat.ambient = cube->ambient;
	constants.mat.diffuse = cube->diffuse;
	constants.mat.specular = cube->specular;
	constants.mat.shininess = cube->shininess;

	return true;
}

void BlinnPhong::update() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Blinn-Phong");

	ImGui::Text("transform");
	ImGui::SliderFloat("yaw", &yaw, -180.0f, 180.0f);
	ImGui::SliderFloat("pitch", &pitch, -180.0f, 180.0f);

	ImGui::End();

	auto rotation = glm::rotate(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));

	const auto translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.5f));

	constants.model = translation * rotation;
	constants.invTr = glm::inverseTranspose(constants.model);
}
