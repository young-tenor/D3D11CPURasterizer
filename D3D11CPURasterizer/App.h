#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Vertex.h"
#include "PixelShader.h"
#include "Mesh.h"

class App {
public:
	virtual ~App() = default;

	virtual bool Init(HWND hWnd);
	virtual void Update() = 0;
	void Render();

private:
	glm::vec2 WorldToScreen(const glm::vec3 &pos);
	float EdgeFunction(const glm::vec2 &v0, const glm::vec2 &v1, const glm::vec2 point);
	void DrawIndexed(int i);
	void CPURender();

protected:
	HWND hWnd = nullptr;
	float width = 1280.0f;
	float height = 720.0f;
	float aspect = 1280.0f / 720.0f;

	ID3D11Device *device = nullptr;
	ID3D11DeviceContext *context = nullptr;
	IDXGISwapChain *swapChain = nullptr;
	ID3D11RenderTargetView *rtv = nullptr;
	D3D11_VIEWPORT viewport = { 0 };

	ID3D11VertexShader *vs = nullptr;
	ID3D11PixelShader *ps = nullptr;

	ID3D11Texture2D *canvas = nullptr;
	ID3D11ShaderResourceView *canvasSrv = nullptr;
	std::vector<glm::vec4> canvasData;
	ID3D11SamplerState *sampler = nullptr;

	std::vector<Vertex> vertexBuffer;
	std::vector<int> indexBuffer;
	std::vector<float> depthBuffer;

	glm::vec4(*pixelShader)(PSInput &) = nullptr;

	float camDist = 1.0f;

	std::vector<Mesh *> meshes;

	bool perspective = false;
	bool perspectiveCorrection = false;
	bool depthBuffering = false;
};