#include "pch.h"
#include "Projection.h"
#include "Vertex.h"
#include "VertexShader.h"

bool Projection::init(HWND h_wnd) {
	if (!App::init(h_wnd)) {
		return false;
	}

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

void Projection::update() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Perspective Projection");

	ImGui::Text("transform");
	ImGui::SliderFloat("Rotation Y", &yaw, -90.0f, 90.0f);

	ImGui::Separator();

	ImGui::Text("projection");
	if (ImGui::RadioButton("Orthographic", !perspective)) {
		perspective = false;
	}
	if (ImGui::RadioButton("Perspective", perspective)) {
		perspective = true;
	}

	ImGui::End();

	std::fill(canvas_data.begin(), canvas_data.end(), glm::vec4(0.1f, 0.2f, 0.4f, 1.0f));

	constants.model = glm::rotate(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));

	for (const auto &mesh : meshes) {
		vertex_buffer.resize(mesh->vertices.size());

		for (int i = 0; i < vertex_buffer.size(); i++) {
			VSInput vs_input;
			vs_input.pos = mesh->vertices[i].pos;
			vs_input.color = mesh->vertices[i].color;

			auto vs_output = vs_main(vs_input);

			vertex_buffer[i].pos = vs_output.pos;
			vertex_buffer[i].color = vs_output.color;
		}

		index_buffer = mesh->indices;

		for (int i = 0; i < index_buffer.size(); i += 3) {
			draw_indexed(i);
		}
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	context->Map(canvas, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, canvas_data.data(), canvas_data.size() * sizeof(glm::vec4));
	context->Unmap(canvas, 0);
}
