#pragma once
#include "App.h"
#include "Mesh.h"

class Projection : public App {
public:
	~Projection() override = default;

	bool init(HWND h_wnd) override;
	void update() override;

private:
	Mesh *rect = nullptr;
	float yaw = 0.0f;
};

