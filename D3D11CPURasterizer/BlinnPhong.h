#pragma once
#include "App.h"
#include "Mesh.h"

class BlinnPhong : public App {
public:
	~BlinnPhong() override = default;

	bool Init(HWND hWnd) override;
	void Update() override;

private:
	Mesh *cube = nullptr;

	float yaw = 0.0f;
	float pitch = 0.0f;
};

