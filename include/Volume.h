#pragma once
#include <vector>
#include <DirectXMath.h>
#include <algorithm>

class Volume {
public:
    Volume(int w, int h, int d, const std::vector<uint16_t>& data);

    float getVoxel(int x, int y, int z) const;
    DirectX::XMFLOAT3 getGradient(int x, int y, int z) const;
    float getInterpolatedVoxel(float x, float y, float z) const;

    int width() const { return m_width; }
    int height() const { return m_height; }
    int depth() const { return m_depth; }

private:
    int m_width, m_height, m_depth;
    std::vector<uint16_t> m_data;
};