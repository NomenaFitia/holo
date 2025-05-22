#include "MarchingCubes.h"
#include <DirectXMath.h>
#include "tables.h"

MarchingCubes::MarchingCubes(const Volume& volume, float isovalue)
    : m_volume(volume), m_isovalue(isovalue) {
}

void MarchingCubes::generateSurface() {
    m_vertices.clear();

    for (int z = 0; z < m_volume.depth() - 1; z++) {
        for (int y = 0; y < m_volume.height() - 1; y++) {
            for (int x = 0; x < m_volume.width() - 1; x++) {
                processCube(x, y, z);
            }
        }
    }
}

void MarchingCubes::processCube(int x, int y, int z) {
    // Récupérer les valeurs des 8 sommets du cube
    float cubeValues[8]{};
    cubeValues[0] = m_volume.getVoxel(x, y, z);
    cubeValues[1] = m_volume.getVoxel(x + 1, y, z);
    cubeValues[2] = m_volume.getVoxel(x + 1, y, z + 1);
    cubeValues[3] = m_volume.getVoxel(x, y, z + 1);
    cubeValues[4] = m_volume.getVoxel(x, y + 1, z);
    cubeValues[5] = m_volume.getVoxel(x + 1, y + 1, z);
    cubeValues[6] = m_volume.getVoxel(x + 1, y + 1, z + 1);
    cubeValues[7] = m_volume.getVoxel(x, y + 1, z + 1);

    // Déterminer l'index du cas
    int cubeIndex = 0;
    if (cubeValues[0] < m_isovalue) cubeIndex |= 1;
    if (cubeValues[1] < m_isovalue) cubeIndex |= 2;
    if (cubeValues[2] < m_isovalue) cubeIndex |= 4;
    if (cubeValues[3] < m_isovalue) cubeIndex |= 8;
    if (cubeValues[4] < m_isovalue) cubeIndex |= 16;
    if (cubeValues[5] < m_isovalue) cubeIndex |= 32;
    if (cubeValues[6] < m_isovalue) cubeIndex |= 64;
    if (cubeValues[7] < m_isovalue) cubeIndex |= 128;

    if (edgeTable[cubeIndex] == 0) return;

    // Positions des sommets du cube
    DirectX::XMFLOAT3 cubeVertices[8] = {
        DirectX::XMFLOAT3(x,   y,   z),
        DirectX::XMFLOAT3(x + 1, y,   z),
        DirectX::XMFLOAT3(x + 1, y,   z + 1),
        DirectX::XMFLOAT3(x,   y,   z + 1),
        DirectX::XMFLOAT3(x,   y + 1, z),
        DirectX::XMFLOAT3(x + 1, y + 1, z),
        DirectX::XMFLOAT3(x + 1, y + 1, z + 1),
        DirectX::XMFLOAT3(x,   y + 1, z + 1)
    };

    // Calculer les vertices d'intersection
    DirectX::XMFLOAT3 vertices[12];
    if (edgeTable[cubeIndex] & 1)    vertices[0] = interpolateVertex(cubeVertices[0], cubeVertices[1], cubeValues[0], cubeValues[1]);
    if (edgeTable[cubeIndex] & 2)    vertices[1] = interpolateVertex(cubeVertices[1], cubeVertices[2], cubeValues[1], cubeValues[2]);
    if (edgeTable[cubeIndex] & 4)    vertices[2] = interpolateVertex(cubeVertices[2], cubeVertices[3], cubeValues[2], cubeValues[3]);
    if (edgeTable[cubeIndex] & 8)    vertices[3] = interpolateVertex(cubeVertices[3], cubeVertices[0], cubeValues[3], cubeValues[0]);
    if (edgeTable[cubeIndex] & 16)   vertices[4] = interpolateVertex(cubeVertices[4], cubeVertices[5], cubeValues[4], cubeValues[5]);
    if (edgeTable[cubeIndex] & 32)   vertices[5] = interpolateVertex(cubeVertices[5], cubeVertices[6], cubeValues[5], cubeValues[6]);
    if (edgeTable[cubeIndex] & 64)   vertices[6] = interpolateVertex(cubeVertices[6], cubeVertices[7], cubeValues[6], cubeValues[7]);
    if (edgeTable[cubeIndex] & 128)  vertices[7] = interpolateVertex(cubeVertices[7], cubeVertices[4], cubeValues[7], cubeValues[4]);
    if (edgeTable[cubeIndex] & 256)  vertices[8] = interpolateVertex(cubeVertices[0], cubeVertices[4], cubeValues[0], cubeValues[4]);
    if (edgeTable[cubeIndex] & 512)  vertices[9] = interpolateVertex(cubeVertices[1], cubeVertices[5], cubeValues[1], cubeValues[5]);
    if (edgeTable[cubeIndex] & 1024) vertices[10] = interpolateVertex(cubeVertices[2], cubeVertices[6], cubeValues[2], cubeValues[6]);
    if (edgeTable[cubeIndex] & 2048) vertices[11] = interpolateVertex(cubeVertices[3], cubeVertices[7], cubeValues[3], cubeValues[7]);

    // Créer les triangles
    for (int i = 0; triTable[cubeIndex][i] != -1; i += 3) {
        Vertex v1, v2, v3;
        v1.position = vertices[triTable[cubeIndex][i]];
        v2.position = vertices[triTable[cubeIndex][i + 1]];
        v3.position = vertices[triTable[cubeIndex][i + 2]];

        // Calculer la normale
        DirectX::XMFLOAT3 normal = calculateNormal(v1.position, v2.position, v3.position);
        v1.normal = v2.normal = v3.normal = normal;

        // Déterminer la couleur basée sur la position
        float r = (v1.position.x + v2.position.x + v3.position.x) / (3.0f * m_volume.width());
        float g = (v1.position.y + v2.position.y + v3.position.y) / (3.0f * m_volume.height());
        float b = (v1.position.z + v2.position.z + v3.position.z) / (3.0f * m_volume.depth());

        v1.color = v2.color = v3.color = DirectX::XMFLOAT3(r, g, b);

        m_vertices.push_back(v1);
        m_vertices.push_back(v2);
        m_vertices.push_back(v3);
    }
}

DirectX::XMFLOAT3 MarchingCubes::interpolateVertex(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2, float val1, float val2) const {
    if (std::abs(m_isovalue - val1) < 0.00001f) return p1;
    if (std::abs(m_isovalue - val2) < 0.00001f) return p2;
    if (std::abs(val1 - val2) < 0.00001f) return p1;

    float mu = (m_isovalue - val1) / (val2 - val1);
    return DirectX::XMFLOAT3(
        p1.x + mu * (p2.x - p1.x),
        p1.y + mu * (p2.y - p1.y),
        p1.z + mu * (p2.z - p1.z)
    );
}

DirectX::XMFLOAT3 MarchingCubes::calculateNormal(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2, const DirectX::XMFLOAT3& p3) const {
    using namespace DirectX;

    XMVECTOR v1 = XMLoadFloat3(&p1);
    XMVECTOR v2 = XMLoadFloat3(&p2);
    XMVECTOR v3 = XMLoadFloat3(&p3);

    XMVECTOR edge1 = v2 - v1;
    XMVECTOR edge2 = v3 - v1;
    XMVECTOR normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));

    DirectX::XMFLOAT3 result;
    XMStoreFloat3(&result, normal);
    return result;
}