#include "pch.h"
#include "Projection.h"
#include "Vertex.h"
#include "VertexShader.h"
#include "PixelShader.h"

bool Projection::Init(HWND hWnd) {
	if (!App::Init(hWnd)) {
		return false;
	}

	pixelShader = PSUVMain;

	rect = new Mesh();

	Vertex v0, v1, v2, v3;

	v0.pos = glm::vec3(-0.5f, 0.5f, 0.0f);
	v1.pos = glm::vec3(0.5f, 0.5f, 0.0f);
	v2.pos = glm::vec3(0.5f, -0.5f, 0.0f);
	v3.pos = glm::vec3(-0.5f, -0.5f, 0.0f);

	v0.color = glm::vec3(1.0f, 0.0f, 0.0f);
	v1.color = glm::vec3(1.0f, 0.0f, 0.0f);
	v2.color = glm::vec3(1.0f, 0.0f, 0.0f);
	v3.color = glm::vec3(1.0f, 0.0f, 0.0f);

	v0.uv = glm::vec2(0.0f, 0.0f);
	v1.uv = glm::vec2(1.0f, 0.0f);
	v2.uv = glm::vec2(1.0f, 1.0f);
	v3.uv = glm::vec2(0.0f, 1.0f);

	rect->vertices.push_back(v0);
	rect->vertices.push_back(v1);
	rect->vertices.push_back(v2);
	rect->vertices.push_back(v3);

	rect->indices.push_back(0);
	rect->indices.push_back(1);
	rect->indices.push_back(2);

	rect->indices.push_back(0);
	rect->indices.push_back(2);
	rect->indices.push_back(3);

	meshes.push_back(rect);

	return true;
}

void Projection::Update() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Perspective Projection");

	ImGui::Text("transform");
	ImGui::SliderFloat("yaw", &yaw, -90.0f, 90.0f);

	ImGui::Separator();

	ImGui::Text("projection");
	if (ImGui::RadioButton("orthographic", !perspective)) {
		perspective = false;
	}
	if (ImGui::RadioButton("perspective", perspective)) {
		perspective = true;
	}
	ImGui::BeginDisabled(!perspective);
	ImGui::Checkbox("perspective correction", &perspectiveCorrection);
	ImGui::EndDisabled();

	ImGui::End();

	std::fill(canvasData.begin(), canvasData.end(), glm::vec4(0.1f, 0.2f, 0.4f, 1.0f));

	constants.model = glm::rotate(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
}
