#pragma once
#include "App.h"
#include "Mesh.h"

class Projection : public App {
public:
	~Projection() override = default;

	bool Init(HWND hWnd) override;
	void Update() override;

private:
	Mesh *rect = nullptr;
	float yaw = 0.0f;
};

