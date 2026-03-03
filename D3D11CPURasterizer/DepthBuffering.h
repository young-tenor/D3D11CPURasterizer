#pragma once
#include "App.h"
#include "Mesh.h"

class DepthBuffering : public App {
public:
	~DepthBuffering() override = default;

	bool init(HWND h_wnd) override;
	void update() override;

public:
	Mesh *front = nullptr;
	Mesh *back = nullptr;
};

