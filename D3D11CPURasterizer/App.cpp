#include "pch.h"
#include "App.h"
#include "VertexShader.h"
#include "PixelShader.h"

bool App::init(HWND h_wnd) {
	this->h_wnd = h_wnd;

	RECT rc;
	GetClientRect(h_wnd, &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;
	aspect = width / height;

	// device & swap chain
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = { 0 };
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = width;
	swap_chain_desc.BufferDesc.Height = height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = h_wnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = TRUE;

	D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		feature_levels,
		1,
		D3D11_SDK_VERSION,
		&swap_chain_desc,
		&swap_chain,
		&device,
		nullptr,
		&context);
	if (FAILED(hr)) {
		std::cout << "D3D11CreateDeviceAndSwapChain() failed." << std::endl;
		return false;
	}

	// render target view
	ID3D11Texture2D *back_buffer = nullptr;
	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&back_buffer);
	if (FAILED(hr)) {
		std::cout << "GetBuffer() failed." << std::endl;
		return false;
	}

	hr = device->CreateRenderTargetView(back_buffer, nullptr, &rtv);
	if (FAILED(hr)) {
		std::cout << "CreateRenderTargetView() failed." << std::endl;
		return false;
	}
	back_buffer->Release();

	// viewport
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	ID3DBlob *vs_blob = nullptr;
	ID3DBlob *ps_blob = nullptr;

	// vertex shader
	hr = D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vs_blob, nullptr);
	if (FAILED(hr)) {
		std::cout << "D3DCompileFromFile() failed: vertex shader" << std::endl;
		return false;
	}

	hr = device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &vs);
	vs_blob->Release();

	// pixel shader
	hr = D3DCompileFromFile(L"Shaders.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &ps_blob, nullptr);
	if (FAILED(hr)) {
		std::cout << "D3DCompileFromFile() failed: pixel shader" << std::endl;
		return false;
	}

	// texture
	hr = device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &ps);
	ps_blob->Release();

	D3D11_TEXTURE2D_DESC texture_desc = { 0 };
	texture_desc.Width = width;
	texture_desc.Height = height;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.Usage = D3D11_USAGE_DYNAMIC;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = device->CreateTexture2D(&texture_desc, nullptr, &canvas);
	if (FAILED(hr)) {
		std::cout << "CreateTexture2D() failed." << std::endl;
		return false;
	}

	hr = device->CreateShaderResourceView(canvas, nullptr, &canvas_srv);
	if (FAILED(hr)) {
		std::cout << "CreateShaderResourceView() failed." << std::endl;
		return false;
	}

	canvas_data.resize(width * height);

	// sampler
	D3D11_SAMPLER_DESC sampler_desc = { 0 };
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&sampler_desc, &sampler);
	if (FAILED(hr)) {
		std::cout << "CreateSamplerState() failed." << std::endl;
		return false;
	}

	// constant buffer
	constants.model = glm::mat4(1.0f);

	// GUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsLight();

	ImGui_ImplWin32_Init(h_wnd);
	ImGui_ImplDX11_Init(device, context);

	return true;
}

void App::render() {
	const float clear_color[] = { 0.1f, 0.2f, 0.4f, 1.0f };
	context->ClearRenderTargetView(rtv, clear_color);

	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &rtv, nullptr);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(vs, nullptr, 0);
	context->PSSetShader(ps, nullptr, 0);

	context->PSSetShaderResources(0, 1, &canvas_srv);
	context->PSSetSamplers(0, 1, &sampler);

	context->Draw(3, 0);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swap_chain->Present(1, 0);
}

// [-1, 1] * [-1, 1] -> [0, width - 1] * [0, height - 1]
glm::vec2 App::world_to_screen(const glm::vec3 &pos) {
	float x, y;
	if (perspective) {
		x = pos.x * cam_dist / (cam_dist + pos.z);
		y = pos.y * cam_dist / (cam_dist + pos.z);
	} else {
		x = pos.x;
		y = pos.y;
	}
	x = x * (width - 1.0f) / 2.0f + (width - 1.0f) / 2.0f;
	y = -y * (height - 1.0f) / 2.0f + (height - 1.0f) / 2.0f;
	return glm::vec2(x, y);
}

float App::edge_function(const glm::vec2 &v0, const glm::vec2 &v1, const glm::vec2 point) {
	const auto a = v0 - point;
	const auto b = v1 - point;
	return a.x * b.y - a.y * b.x;
}

void App::draw_indexed(int idx) {
	const int i0 = index_buffer[idx];
	const int i1 = index_buffer[idx + 1];
	const int i2 = index_buffer[idx + 2];
	
	const auto v0 = world_to_screen(vertex_buffer[i0].pos);
	const auto v1 = world_to_screen(vertex_buffer[i1].pos);
	const auto v2 = world_to_screen(vertex_buffer[i2].pos);

	const auto &c0 = vertex_buffer[i0].color;
	const auto &c1 = vertex_buffer[i1].color;
	const auto &c2 = vertex_buffer[i2].color;

	const auto uv0 = vertex_buffer[i0].uv;
	const auto uv1 = vertex_buffer[i1].uv;
	const auto uv2 = vertex_buffer[i2].uv;

	const int min_x = std::min({ v0.x, v1.x, v2.x });
	const int max_x = std::max({ v0.x, v1.x, v2.x });
	const int min_y = std::min({ v0.y, v1.y, v2.y });
	const int max_y = std::max({ v0.y, v1.y, v2.y });

	for (int i = min_y; i < max_y; i++) {
		for (int j = min_x; j < max_x; j++) {
			const auto point = glm::vec2((float)j + 0.5f, (float)i + 0.5f);

			float w0 = edge_function(v1, v2, point);
			float w1 = edge_function(v2, v0, point);
			float w2 = edge_function(v0, v1, point);

			if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
				// https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf
				if (perspective && perspective_correction) {
					float z0 = cam_dist + vertex_buffer[i0].pos.z;
					float z1 = cam_dist + vertex_buffer[i1].pos.z;
					float z2 = cam_dist + vertex_buffer[i2].pos.z;

					w0 /= z0;
					w1 /= z1;
					w2 /= z2;
				}

				const float w_sum = w0 + w1 + w2;

				const auto color = (w0 * c0 + w1 * c1 + w2 * c2) / w_sum;
				const auto uv = (w0 * uv0 + w1 * uv1 + w2 * uv2) / w_sum;

				PSInput ps_input;
				ps_input.color = color;
				ps_input.uv = uv;

				canvas_data[i * width + j] = pixel_shader(ps_input);
			}
		}
	}
}
