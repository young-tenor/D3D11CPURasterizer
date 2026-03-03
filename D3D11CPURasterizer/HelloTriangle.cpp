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

	std::fill(canvas_data.begin(), canvas_data.end(), glm::vec4(0.1f, 0.2f, 0.4f, 1.0f));

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

		draw_indexed(0);
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	context->Map(canvas, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, canvas_data.data(), canvas_data.size() * sizeof(glm::vec4));
	context->Unmap(canvas, 0);
}
