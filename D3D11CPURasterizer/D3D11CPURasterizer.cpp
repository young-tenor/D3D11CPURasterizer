#include "pch.h"
#include "App.h"
#include "HelloTriangle.h"
#include "Projection.h"
#include "DepthBuffering.h"
#include "BlinnPhong.h"

HINSTANCE hInst;
HWND hWnd;
const wchar_t szTitle[] = L"D3D11 CPU Ray Tracer";
const wchar_t szWindowClass[] = L"D3D11CPURayTracer";

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main() {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, SW_SHOWNORMAL)) {
		return -1;
	}

	const auto hello_triangle = new HelloTriangle();
	const auto projection = new Projection();
	const auto depth_buffering = new DepthBuffering();
	const auto blinn_phong = new BlinnPhong();

	App *app = blinn_phong;

	if (!app->init(hWnd)) {
		std::cout << "init() failed." << std::endl;
		return -1;
	}

	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			app->update();
			app->render();
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.lpszClassName = szWindowClass;

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance;

	RECT rc = { 0, 0, 1280, 720 };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowW(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}