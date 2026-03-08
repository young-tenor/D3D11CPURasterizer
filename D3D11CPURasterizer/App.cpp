#include "pch.h"
#include "App.h"
#include "VertexShader.h"
#include "PixelShader.h"

bool App::Init(HWND hWnd) {
	this->hWnd = hWnd;

	RECT rc;
	GetClientRect(hWnd, &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;
	aspect = width / height;

	// device & swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain,
		&device,
		nullptr,
		&context);
	if (FAILED(hr)) {
		std::cout << "D3D11CreateDeviceAndSwapChain() failed." << std::endl;
		return false;
	}

	// render target view
	ID3D11Texture2D *backBuffer = nullptr;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backBuffer);
	if (FAILED(hr)) {
		std::cout << "GetBuffer() failed." << std::endl;
		return false;
	}

	hr = device->CreateRenderTargetView(backBuffer, nullptr, &rtv);
	if (FAILED(hr)) {
		std::cout << "CreateRenderTargetView() failed." << std::endl;
		return false;
	}
	backBuffer->Release();

	// viewport
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	ID3DBlob *vsBlob = nullptr;
	ID3DBlob *psBlob = nullptr;

	// vertex shader
	hr = D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vsBlob, nullptr);
	if (FAILED(hr)) {
		std::cout << "D3DCompileFromFile() failed: vertex shader" << std::endl;
		return false;
	}

	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vs);
	vsBlob->Release();

	// pixel shader
	hr = D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, nullptr);
	if (FAILED(hr)) {
		std::cout << "D3DCompileFromFile() failed: pixel shader" << std::endl;
		return false;
	}

	hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &ps);
	psBlob->Release();

	// texture
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = device->CreateTexture2D(&textureDesc, nullptr, &canvas);
	if (FAILED(hr)) {
		std::cout << "CreateTexture2D() failed." << std::endl;
		return false;
	}

	hr = device->CreateShaderResourceView(canvas, nullptr, &canvasSrv);
	if (FAILED(hr)) {
		std::cout << "CreateShaderResourceView() failed." << std::endl;
		return false;
	}

	canvasData.resize(width * height);

	// sampler
	D3D11_SAMPLER_DESC samplerDesc = { 0 };
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&samplerDesc, &sampler);
	if (FAILED(hr)) {
		std::cout << "CreateSamplerState() failed." << std::endl;
		return false;
	}

	// depth buffer
	depthBuffer.resize(width * height);

	// constant buffer
	constants.model = glm::mat4(1.0f);
	constants.invTr = glm::mat4(1.0f);
	constants.light.pos = glm::vec3(0.0f, 1.0f, -1.0f);
	constants.light.strength = 1.0f;

	// GUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsLight();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device, context);

	return true;
}

void App::Render() {
	CPURender();

	const float clearColor[] = { 0.1f, 0.2f, 0.4f, 1.0f };
	context->ClearRenderTargetView(rtv, clearColor);

	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &rtv, nullptr);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(vs, nullptr, 0);
	context->PSSetShader(ps, nullptr, 0);

	context->PSSetShaderResources(0, 1, &canvasSrv);
	context->PSSetSamplers(0, 1, &sampler);

	context->Draw(3, 0);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swapChain->Present(1, 0);
}

// [-aspect, aspect] * [-1, 1] -> [-1, 1] * [-1, 1] -> [0, width - 1] * [0, height - 1]
glm::vec2 App::WorldToScreen(const glm::vec3 &pos) {
	// world to ndc
	auto posNdc = glm::vec2(pos.x / aspect, pos.y);
	if (perspective) {
		posNdc = posNdc * camDist / (camDist + pos.z);
	}

	// ndc to screen
	auto posScreen = posNdc;
	const float xHalf = (width - 1.0f) * 0.5f;
	const float yHalf = (height - 1.0f) * 0.5f;
	posScreen = posScreen * glm::vec2(xHalf, -yHalf) + glm::vec2(xHalf, yHalf);

	return posScreen;
}

float App::EdgeFunction(const glm::vec2 &v0, const glm::vec2 &v1, const glm::vec2 point) {
	const auto a = v0 - point;
	const auto b = v1 - point;
	return a.x * b.y - a.y * b.x;
}

void App::DrawIndexed(int idx) {
	// index
	const int i0 = indexBuffer[idx];
	const int i1 = indexBuffer[idx + 1];
	const int i2 = indexBuffer[idx + 2];

	// world position
	const auto world0 = vertexBuffer[i0].pos;
	const auto world1 = vertexBuffer[i1].pos;
	const auto world2 = vertexBuffer[i2].pos;
	
	// screen position
	const auto screen0 = WorldToScreen(world0);
	const auto screen1 = WorldToScreen(world1);
	const auto screen2 = WorldToScreen(world2);

	// color
	const auto &c0 = vertexBuffer[i0].color;
	const auto &c1 = vertexBuffer[i1].color;
	const auto &c2 = vertexBuffer[i2].color;

	// uv
	const auto uv0 = vertexBuffer[i0].uv;
	const auto uv1 = vertexBuffer[i1].uv;
	const auto uv2 = vertexBuffer[i2].uv;

	// normal
	const auto n0 = vertexBuffer[i0].normal;
	const auto n1 = vertexBuffer[i1].normal;
	const auto n2 = vertexBuffer[i2].normal;

	const int minX = std::min({ screen0.x, screen1.x, screen2.x });
	const int maxX = std::max({ screen0.x, screen1.x, screen2.x });
	const int minY = std::min({ screen0.y, screen1.y, screen2.y });
	const int maxY = std::max({ screen0.y, screen1.y, screen2.y });

	for (int i = minY; i < maxY; i++) {
		for (int j = minX; j < maxX; j++) {
			const auto point = glm::vec2((float)j + 0.5f, (float)i + 0.5f);

			float w0 = EdgeFunction(screen1, screen2, point);
			float w1 = EdgeFunction(screen2, screen0, point);
			float w2 = EdgeFunction(screen0, screen1, point);

			if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
				// https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf
				if (perspective && perspectiveCorrection) {
					float z0 = camDist + vertexBuffer[i0].pos.z;
					float z1 = camDist + vertexBuffer[i1].pos.z;
					float z2 = camDist + vertexBuffer[i2].pos.z;

					w0 /= z0;
					w1 /= z1;
					w2 /= z2;
				}

				const float wSum = w0 + w1 + w2;

				const auto posWorld = (w0 * world0 + w1 * world1 + w2 * world2) / wSum;
				const auto color = (w0 * c0 + w1 * c1 + w2 * c2) / wSum;
				const auto uv = (w0 * uv0 + w1 * uv1 + w2 * uv2) / wSum;
				const auto normal = glm::normalize((w0 * n0 + w1 * n1 + w2 * n2) / wSum);

				PSInput psInput;
				psInput.posWolrd = posWorld;
				psInput.color = color;
				psInput.uv = uv;
				psInput.normal = normal;

				if (depthBuffering) {
					const float depth = (w0 * vertexBuffer[i0].pos.z + w1 * vertexBuffer[i1].pos.z + w2 * vertexBuffer[i2].pos.z) / wSum;
					if (depth < depthBuffer[i * width + j]) {
						canvasData[i * width + j] = pixelShader(psInput);
						depthBuffer[i * width + j] = depth;
					}
				} else {
					canvasData[i * width + j] = pixelShader(psInput);
				}
			}
		}
	}
}

void App::CPURender() {
	std::fill(canvasData.begin(), canvasData.end(), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	std::fill(depthBuffer.begin(), depthBuffer.end(), 10.0f);

	for (const auto &mesh : meshes) {
		vertexBuffer.resize(mesh->vertices.size());

		for (int i = 0; i < vertexBuffer.size(); i++) {
			VSInput vsInput;
			vsInput.pos = mesh->vertices[i].pos;
			vsInput.color = mesh->vertices[i].color;
			vsInput.uv = mesh->vertices[i].uv;
			vsInput.normal = mesh->vertices[i].normal;

			auto vsOutput = VSMain(vsInput);

			vertexBuffer[i].pos = vsOutput.pos;
			vertexBuffer[i].color = vsOutput.color;
			vertexBuffer[i].uv = vsOutput.uv;
			vertexBuffer[i].normal = vsOutput.normal;
		}

		indexBuffer = mesh->indices;

		for (int i = 0; i < indexBuffer.size(); i += 3) {
			DrawIndexed(i);
		}
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	context->Map(canvas, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, canvasData.data(), canvasData.size() * sizeof(glm::vec4));
	context->Unmap(canvas, 0);
}