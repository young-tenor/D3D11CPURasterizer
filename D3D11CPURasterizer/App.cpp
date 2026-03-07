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

	hr = device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &ps);
	ps_blob->Release();

	// texture
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

	// depth buffer
	depth_buffer.resize(width * height);

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

	ImGui_ImplWin32_Init(h_wnd);
	ImGui_ImplDX11_Init(device, context);

	return true;
}

void App::render() {
	cpu_render();

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

// [-aspect, aspect] * [-1, 1] -> [-1, 1] * [-1, 1] -> [0, width - 1] * [0, height - 1]
glm::vec2 App::world_to_screen(const glm::vec3 &pos) {
	// world to ndc
	auto pos_ndc = glm::vec2(pos.x / aspect, pos.y);
	if (perspective) {
		pos_ndc = pos_ndc * cam_dist / (cam_dist + pos.z);
	}

	// ndc to screen
	auto pos_screen = pos_ndc;
	const float x_half = (width - 1.0f) * 0.5f;
	const float y_half = (height - 1.0f) * 0.5f;
	pos_screen = pos_screen * glm::vec2(x_half, -y_half) + glm::vec2(x_half, y_half);

	return pos_screen;
}

float App::edge_function(const glm::vec2 &v0, const glm::vec2 &v1, const glm::vec2 point) {
	const auto a = v0 - point;
	const auto b = v1 - point;
	return a.x * b.y - a.y * b.x;
}

void App::draw_indexed(int idx) {
	// index
	const int i0 = index_buffer[idx];
	const int i1 = index_buffer[idx + 1];
	const int i2 = index_buffer[idx + 2];

	// world position
	const auto world0 = vertex_buffer[i0].pos;
	const auto world1 = vertex_buffer[i1].pos;
	const auto world2 = vertex_buffer[i2].pos;
	
	// screen position
	const auto screen0 = world_to_screen(world0);
	const auto screen1 = world_to_screen(world1);
	const auto screen2 = world_to_screen(world2);

	// color
	const auto &c0 = vertex_buffer[i0].color;
	const auto &c1 = vertex_buffer[i1].color;
	const auto &c2 = vertex_buffer[i2].color;

	// uv
	const auto uv0 = vertex_buffer[i0].uv;
	const auto uv1 = vertex_buffer[i1].uv;
	const auto uv2 = vertex_buffer[i2].uv;

	// normal
	const auto n0 = vertex_buffer[i0].normal;
	const auto n1 = vertex_buffer[i1].normal;
	const auto n2 = vertex_buffer[i2].normal;

	const int min_x = std::min({ screen0.x, screen1.x, screen2.x });
	const int max_x = std::max({ screen0.x, screen1.x, screen2.x });
	const int min_y = std::min({ screen0.y, screen1.y, screen2.y });
	const int max_y = std::max({ screen0.y, screen1.y, screen2.y });

	for (int i = min_y; i < max_y; i++) {
		for (int j = min_x; j < max_x; j++) {
			const auto point = glm::vec2((float)j + 0.5f, (float)i + 0.5f);

			float w0 = edge_function(screen1, screen2, point);
			float w1 = edge_function(screen2, screen0, point);
			float w2 = edge_function(screen0, screen1, point);

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

				const auto pos_world = (w0 * world0 + w1 * world1 + w2 * world2) / w_sum;
				const auto color = (w0 * c0 + w1 * c1 + w2 * c2) / w_sum;
				const auto uv = (w0 * uv0 + w1 * uv1 + w2 * uv2) / w_sum;
				const auto normal = glm::normalize((w0 * n0 + w1 * n1 + w2 * n2) / w_sum);

				PSInput ps_input;
				ps_input.pos_world = pos_world;
				ps_input.color = color;
				ps_input.uv = uv;
				ps_input.normal = normal;

				if (depth_buffering) {
					const float depth = (w0 * vertex_buffer[i0].pos.z + w1 * vertex_buffer[i1].pos.z + w2 * vertex_buffer[i2].pos.z) / w_sum;
					if (depth < depth_buffer[i * width + j]) {
						canvas_data[i * width + j] = pixel_shader(ps_input);
						depth_buffer[i * width + j] = depth;
					}
				} else {
					canvas_data[i * width + j] = pixel_shader(ps_input);
				}
			}
		}
	}
}

void App::cpu_render() {
	std::fill(canvas_data.begin(), canvas_data.end(), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	std::fill(depth_buffer.begin(), depth_buffer.end(), 10.0f);

	for (const auto &mesh : meshes) {
		vertex_buffer.resize(mesh->vertices.size());

		for (int i = 0; i < vertex_buffer.size(); i++) {
			VSInput vs_input;
			vs_input.pos = mesh->vertices[i].pos;
			vs_input.color = mesh->vertices[i].color;
			vs_input.uv = mesh->vertices[i].uv;
			vs_input.normal = mesh->vertices[i].normal;

			auto vs_output = vs_main(vs_input);

			vertex_buffer[i].pos = vs_output.pos;
			vertex_buffer[i].color = vs_output.color;
			vertex_buffer[i].uv = vs_output.uv;
			vertex_buffer[i].normal = vs_output.normal;
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