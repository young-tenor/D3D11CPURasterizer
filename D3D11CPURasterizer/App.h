#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Vertex.h"
#include "PixelShader.h"
#include "Mesh.h"

class App {
public:
	virtual ~App() = default;

	virtual bool init(HWND h_wnd);
	virtual void update() = 0;
	void render();

private:
	glm::vec2 world_to_screen(const glm::vec3 &pos);
	float edge_function(const glm::vec2 &v0, const glm::vec2 &v1, const glm::vec2 point);
	void draw_indexed(int i);
	void cpu_render();

protected:
	HWND h_wnd = nullptr;
	float width = 1280.0f;
	float height = 720.0f;
	float aspect = 1280.0f / 720.0f;

	ID3D11Device *device = nullptr;
	ID3D11DeviceContext *context = nullptr;
	IDXGISwapChain *swap_chain = nullptr;
	ID3D11RenderTargetView *rtv = nullptr;
	D3D11_VIEWPORT viewport = { 0 };

	ID3D11VertexShader *vs = nullptr;
	ID3D11PixelShader *ps = nullptr;

	ID3D11Texture2D *canvas = nullptr;
	ID3D11ShaderResourceView *canvas_srv = nullptr;
	std::vector<glm::vec4> canvas_data;
	ID3D11SamplerState *sampler = nullptr;

	std::vector<Vertex> vertex_buffer;
	std::vector<int> index_buffer;
	std::vector<float> depth_buffer;

	glm::vec4(*pixel_shader)(PSInput &) = nullptr;

	float cam_dist = 1.0f;

	std::vector<Mesh *> meshes;

	bool perspective = false;
	bool perspective_correction = false;
	bool depth_buffering = false;
};