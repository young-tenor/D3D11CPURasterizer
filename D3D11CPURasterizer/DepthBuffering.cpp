#include "pch.h"
#include "DepthBuffering.h"
#include "Vertex.h"
#include "VertexShader.h"
#include "PixelShader.h"

bool DepthBuffering::init(HWND h_wnd) {
	if (!App::init(h_wnd)) {
		return false;
	}

	perspective = true;
	perspective_correction = true;

	pixel_shader = ps_color_main;

	front = new Mesh();
	{
		Vertex v0, v1, v2;

		v0.pos = glm::vec3(-0.33f, 0.5f, 0.0f);
		v1.pos = glm::vec3(-0.33f + 0.5f, -0.5f, 0.0f);
		v2.pos = glm::vec3(-0.33f - 0.5f, -0.5f, 0.0f);

		v0.color = glm::vec3(1.0f, 0.0f, 0.0f);
		v1.color = glm::vec3(1.0f, 0.0f, 0.0f);
		v2.color = glm::vec3(1.0f, 0.0f, 0.0f);

		front->vertices.push_back(v0);
		front->vertices.push_back(v1);
		front->vertices.push_back(v2);
	}
	front->indices.push_back(0);
	front->indices.push_back(1);
	front->indices.push_back(2);

	meshes.push_back(front);

	back = new Mesh();
	{
		back = new Mesh();

		Vertex v0, v1, v2;

		v0.pos = glm::vec3(0.33f, 0.5f, 0.5f);
		v1.pos = glm::vec3(0.33f + 0.5f, -0.5f, 0.5f);
		v2.pos = glm::vec3(0.33f - 0.5f, -0.5f, 0.5f);

		v0.color = glm::vec3(0.0f, 0.0f, 1.0f);
		v1.color = glm::vec3(0.0f, 0.0f, 1.0f);
		v2.color = glm::vec3(0.0f, 0.0f, 1.0f);

		back->vertices.push_back(v0);
		back->vertices.push_back(v1);
		back->vertices.push_back(v2);
	}
	back->indices.push_back(0);
	back->indices.push_back(1);
	back->indices.push_back(2);

	meshes.push_back(back);

	return true;
}

void DepthBuffering::update() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Depth Buffer");

	ImGui::Checkbox("use depth buffer", &depth_buffering);

	ImGui::End();

	std::fill(canvas_data.begin(), canvas_data.end(), glm::vec4(0.1f, 0.2f, 0.4f, 1.0f));
	std::fill(depth_buffer.begin(), depth_buffer.end(), 10.0f);

	for (const auto &mesh : meshes) {
		vertex_buffer.resize(mesh->vertices.size());

		for (int i = 0; i < vertex_buffer.size(); i++) {
			VSInput vs_input;
			vs_input.pos = mesh->vertices[i].pos;
			vs_input.color = mesh->vertices[i].color;
			vs_input.uv = mesh->vertices[i].uv;

			auto vs_output = vs_main(vs_input);

			vertex_buffer[i].pos = vs_output.pos;
			vertex_buffer[i].color = vs_output.color;
			vertex_buffer[i].uv = vs_output.uv;
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
