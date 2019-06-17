#pragma once
#include "stdafx.h"
#include "resource.h"

namespace fl{
	void Setup();
	bool InitWindow(const wstring& title = L"Program", int xpos = 20, int ypos = 20, int width = 1024, int height = 768);
}