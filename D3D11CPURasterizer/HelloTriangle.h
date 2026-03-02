#pragma once
#include "App.h"
#include "Mesh.h"

class HelloTriangle : public App {
public:
	~HelloTriangle() override = default;

	bool init(HWND h_wnd) override;
	void update() override;

private:
	Mesh *triangle = nullptr;
};

