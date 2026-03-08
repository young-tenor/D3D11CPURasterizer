#pragma once
#include "App.h"
#include "Mesh.h"

class DepthBuffering : public App {
public:
	~DepthBuffering() override = default;

	bool Init(HWND hWnd) override;
	void Update() override;

public:
	Mesh *front = nullptr;
	Mesh *back = nullptr;
};

