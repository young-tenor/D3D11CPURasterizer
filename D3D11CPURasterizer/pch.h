#pragma once
#define NOMINMAX

#include <algorithm>
#include <cmath>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")