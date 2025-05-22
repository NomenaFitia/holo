#include "volume.h"

Volume::Volume(int w, int h, int d, const std::vector<uint16_t>& data)
    : m_width(w), m_height(h), m_depth(d), m_data(data) {
}

float Volume::getVoxel(int x, int y, int z) const {
    x = std::clamp(x, 0, m_width - 1);
    y = std::clamp(y, 0, m_height - 1);
    z = std::clamp(z, 0, m_depth - 1);
    return m_data[x + y * m_width + z * m_width * m_height];
}

DirectX::XMFLOAT3 Volume::getGradient(int x, int y, int z) const {
    float gx = getVoxel(x + 1, y, z) - getVoxel(x - 1, y, z);
    float gy = getVoxel(x, y + 1, z) - getVoxel(x, y - 1, z);
    float gz = getVoxel(x, y, z + 1) - getVoxel(x, y, z - 1);
    return DirectX::XMFLOAT3(gx, gy, gz);
}

float Volume::getInterpolatedVoxel(float x, float y, float z) const {
    int x0 = static_cast<int>(floor(x));
    int y0 = static_cast<int>(floor(y));
    int z0 = static_cast<int>(floor(z));
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    int z1 = z0 + 1;

    // Clamp coordinates
    x0 = std::clamp(x0, 0, m_width - 1);
    y0 = std::clamp(y0, 0, m_height - 1);
    z0 = std::clamp(z0, 0, m_depth - 1);
    x1 = std::clamp(x1, 0, m_width - 1);
    y1 = std::clamp(y1, 0, m_height - 1);
    z1 = std::clamp(z1, 0, m_depth - 1);

    float xd = x - x0;
    float yd = y - y0;
    float zd = z - z0;

    // Interpolation le long de l'axe X
    float c00 = getVoxel(x0, y0, z0) * (1 - xd) + getVoxel(x1, y0, z0) * xd;
    float c01 = getVoxel(x0, y0, z1) * (1 - xd) + getVoxel(x1, y0, z1) * xd;
    float c10 = getVoxel(x0, y1, z0) * (1 - xd) + getVoxel(x1, y1, z0) * xd;
    float c11 = getVoxel(x0, y1, z1) * (1 - xd) + getVoxel(x1, y1, z1) * xd;

    // Interpolation le long de l'axe Y
    float c0 = c00 * (1 - yd) + c10 * yd;
    float c1 = c01 * (1 - yd) + c11 * yd;

    // Interpolation le long de l'axe Z
    return c0 * (1 - zd) + c1 * zd;
}