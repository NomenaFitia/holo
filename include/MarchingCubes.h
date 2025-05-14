#pragma once
#include <DirectXMath.h>
#include <vector>

struct Vertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 color;
};

class MarchingCubes {
public:
    //std::vector<Vertex> GenerateSurface(const std::vector<float>& volume, int width, int height, int depth);

private:
    static const int edgeTable[256];
    static const int triTable[256][16];
};