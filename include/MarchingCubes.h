#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Volume.h"

struct Vertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT3 color;
};

class MarchingCubes {
public:
    MarchingCubes(const Volume& volume, float isovalue);

    const std::vector<Vertex>& getVertices() const { return m_vertices; }
    void generateSurface();

private:
    void processCube(int x, int y, int z);
    DirectX::XMFLOAT3 interpolateVertex(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2, float val1, float val2) const;
    DirectX::XMFLOAT3 calculateNormal(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2, const DirectX::XMFLOAT3& p3) const;

    const Volume& m_volume;
    float m_isovalue;
    std::vector<Vertex> m_vertices;
};