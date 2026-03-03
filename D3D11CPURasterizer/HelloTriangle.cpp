#include "pch.h"
#include "HelloTriangle.h"
#include "Vertex.h"
#include "VertexShader.h"
#include "PixelShader.h"

bool HelloTriangle::init(HWND h_wnd) {
	if (!App::init(h_wnd)) {
		return false;
	}

	pixel_shader = ps_color_main;

	triangle = new Mesh();

	Vertex v0, v1, v2;

	v0.pos = glm::vec3(0.0f, 0.5f, 0.0f);
	v1.pos = glm::vec3(0.5f, -0.5f, 0.0f);
	v2.pos = glm::vec3(-0.5f, -0.5f, 0.0f);

	v0.color = glm::vec3(1.0f, 0.0f, 0.0f);
	v1.color = glm::vec3(0.0f, 1.0f, 0.0f);
	v2.color = glm::vec3(0.0f, 0.0f, 1.0f);

	triangle->vertices.push_back(v0);
	triangle->vertices.push_back(v1);
	triangle->vertices.push_back(v2);

	triangle->indices.push_back(0);
	triangle->indices.push_back(1);
	triangle->indices.push_back(2);

	meshes.push_back(triangle);

	return true;
}

void HelloTriangle::update() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Hello Triangle");

	ImGui::End();
}
