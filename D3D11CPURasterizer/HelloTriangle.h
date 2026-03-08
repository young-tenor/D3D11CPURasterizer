#pragma once
#include "App.h"
#include "Mesh.h"

class HelloTriangle : public App {
public:
	~HelloTriangle() override = default;

	bool Init(HWND hWnd) override;
	void Update() override;

private:
	Mesh *triangle = nullptr;
};

