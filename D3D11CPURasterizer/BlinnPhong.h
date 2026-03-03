#pragma once
#include "App.h"
#include "Mesh.h"

class BlinnPhong : public App {
public:
	~BlinnPhong() override = default;

	bool init(HWND h_wnd) override;
	void update() override;

private:
	Mesh *cube = nullptr;

	float yaw = 0.0f;
	float pitch = 0.0f;
};

