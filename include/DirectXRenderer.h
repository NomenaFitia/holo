#pragma once
#include <d3d11.h>
#include <vector>
#include "MarchingCubes.h"

class DirectXRenderer {
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;

public:
    //void Init(HWND hwnd);
    //void Render(const std::vector<Vertex>& vertices);
};