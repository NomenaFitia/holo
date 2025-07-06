#include "MarchingCubes.h"
#include "tables.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <array>

float MarchingCubes::interpolate(float val1, float val2, uint16_t isovalue, float x1, float x2) {
    if (std::abs(isovalue - val1) < 0.00001f) return x1;
    if (std::abs(isovalue - val2) < 0.00001f) return x2;
    if (std::abs(val1 - val2) < 0.00001f) return x1;
    return x1 + (isovalue - val1) * (x2 - x1) / (val2 - val1);
}

uint8_t MarchingCubes::computeCubeIndex(const VolumeData& volumeData, int x, int y, int z, uint16_t isovalue) {
    uint8_t cubeIndex = 0;

    if (x >= volumeData.width - 1 || y >= volumeData.height - 1 || z >= volumeData.depth - 1) {
        return 0;
    }

    uint16_t values[8];
    values[0] = volumeData.voxels[x + y * volumeData.width + z * volumeData.width * volumeData.height];
    values[1] = volumeData.voxels[(x + 1) + y * volumeData.width + z * volumeData.width * volumeData.height];
    values[2] = volumeData.voxels[(x + 1) + (y + 1) * volumeData.width + z * volumeData.width * volumeData.height];
    values[3] = volumeData.voxels[x + (y + 1) * volumeData.width + z * volumeData.width * volumeData.height];
    values[4] = volumeData.voxels[x + y * volumeData.width + (z + 1) * volumeData.width * volumeData.height];
    values[5] = volumeData.voxels[(x + 1) + y * volumeData.width + (z + 1) * volumeData.width * volumeData.height];
    values[6] = volumeData.voxels[(x + 1) + (y + 1) * volumeData.width + (z + 1) * volumeData.width * volumeData.height];
    values[7] = volumeData.voxels[x + (y + 1) * volumeData.width + (z + 1) * volumeData.width * volumeData.height];

    for (int i = 0; i < 8; ++i) {
        if (values[i] < isovalue) {
            cubeIndex |= (1 << i);
        }
    }

    return cubeIndex;
}

void MarchingCubes::processCube(const VolumeData& volumeData, int x, int y, int z, uint16_t isovalue, std::vector<Triangle>& triangles) {

    uint8_t cubeIndex = computeCubeIndex(volumeData, x, y, z, isovalue);
    int edges = edgeTable[cubeIndex];

    if (edges == 0) {
        return;
    }

    uint16_t values[8];
    values[0] = volumeData.voxels[x + y * volumeData.width + z * volumeData.width * volumeData.height];
    values[1] = volumeData.voxels[(x + 1) + y * volumeData.width + z * volumeData.width * volumeData.height];
    values[2] = volumeData.voxels[(x + 1) + (y + 1) * volumeData.width + z * volumeData.width * volumeData.height];
    values[3] = volumeData.voxels[x + (y + 1) * volumeData.width + z * volumeData.width * volumeData.height];
    values[4] = volumeData.voxels[x + y * volumeData.width + (z + 1) * volumeData.width * volumeData.height];
    values[5] = volumeData.voxels[(x + 1) + y * volumeData.width + (z + 1) * volumeData.width * volumeData.height];
    values[6] = volumeData.voxels[(x + 1) + (y + 1) * volumeData.width + (z + 1) * volumeData.width * volumeData.height];
    values[7] = volumeData.voxels[x + (y + 1) * volumeData.width + (z + 1) * volumeData.width * volumeData.height];

    float vertices[12][3];

    if (edges & 0x001) {
        vertices[0][0] = x + interpolate(values[0], values[1], isovalue, 0.0f, 1.0f);
        vertices[0][1] = y;
        vertices[0][2] = z;
    }
    if (edges & 0x002) {
        vertices[1][0] = x + 1;
        vertices[1][1] = y + interpolate(values[1], values[2], isovalue, 0.0f, 1.0f);
        vertices[1][2] = z;
    }
    if (edges & 0x004) {
        vertices[2][0] = x + interpolate(values[3], values[2], isovalue, 0.0f, 1.0f);
        vertices[2][1] = y + 1;
        vertices[2][2] = z;
    }
    if (edges & 0x008) {
        vertices[3][0] = x;
        vertices[3][1] = y + interpolate(values[0], values[3], isovalue, 0.0f, 1.0f);
        vertices[3][2] = z;
    }
    if (edges & 0x010) {
        vertices[4][0] = x + interpolate(values[4], values[5], isovalue, 0.0f, 1.0f);
        vertices[4][1] = y;
        vertices[4][2] = z + 1;
    }
    if (edges & 0x020) {
        vertices[5][0] = x + 1;
        vertices[5][1] = y + interpolate(values[5], values[6], isovalue, 0.0f, 1.0f);
        vertices[5][2] = z + 1;
    }
    if (edges & 0x040) {
        vertices[6][0] = x + interpolate(values[7], values[6], isovalue, 0.0f, 1.0f);
        vertices[6][1] = y + 1;
        vertices[6][2] = z + 1;
    }
    if (edges & 0x080) {
        vertices[7][0] = x;
        vertices[7][1] = y + interpolate(values[4], values[7], isovalue, 0.0f, 1.0f);
        vertices[7][2] = z + 1;
    }
    if (edges & 0x100) {
        vertices[8][0] = x;
        vertices[8][1] = y;
        vertices[8][2] = z + interpolate(values[0], values[4], isovalue, 0.0f, 1.0f);
    }
    if (edges & 0x200) {
        vertices[9][0] = x + 1;
        vertices[9][1] = y;
        vertices[9][2] = z + interpolate(values[1], values[5], isovalue, 0.0f, 1.0f);
    }
    if (edges & 0x400) {
        vertices[10][0] = x + 1;
        vertices[10][1] = y + 1;
        vertices[10][2] = z + interpolate(values[2], values[6], isovalue, 0.0f, 1.0f);
    }
    if (edges & 0x800) {
        vertices[11][0] = x;
        vertices[11][1] = y + 1;
        vertices[11][2] = z + interpolate(values[3], values[7], isovalue, 0.0f, 1.0f);
    }

    for (int i = 0; triTable[cubeIndex][i] != -1; i += 3) {
        Triangle triangle;
        int v0 = triTable[cubeIndex][i];
        int v1 = triTable[cubeIndex][i + 1];
        int v2 = triTable[cubeIndex][i + 2];

        for (int j = 0; j < 3; j++) {
            triangle.v1[j] = vertices[v0][j];
            triangle.v2[j] = vertices[v1][j];
            triangle.v3[j] = vertices[v2][j];
        }

        triangles.push_back(triangle);
    }
}

std::vector<Triangle> MarchingCubes::generateSurface(const VolumeData& volumeData, uint16_t isovalue) {
    std::vector<Triangle> triangles;

    for (int z = 0; z < volumeData.depth - 1; ++z) {
        for (int y = 0; y < volumeData.height - 1; ++y) {
            for (int x = 0; x < volumeData.width - 1; ++x) {
                processCube(volumeData, x, y, z, isovalue, triangles);
            }
        }
    }

    return triangles;
}

void MarchingCubes::saveToObj(const std::vector<Triangle>& triangles, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    for (const auto& triangle : triangles) {
        file << "v " << triangle.v1[0] << " " << triangle.v1[1] << " " << triangle.v1[2] << "\n";
        file << "v " << triangle.v2[0] << " " << triangle.v2[1] << " " << triangle.v2[2] << "\n";
        file << "v " << triangle.v3[0] << " " << triangle.v3[1] << " " << triangle.v3[2] << "\n";
    }

    for (size_t i = 0; i < triangles.size(); ++i) {
        file << "f " << 3 * i + 1 << " " << 3 * i + 2 << " " << 3 * i + 3 << "\n";
    }

    file.close();
}

/*

calcul les normales dans la creation du fichier , a verifier si c'est mieux de calculer dans une autre struct

*/
void MarchingCubes::saveToObjWithNormals(const std::vector<Triangle>& triangles, const std::string& filename) 
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    // Écriture des sommets et calcul des normales
    std::vector<std::array<float, 3>> normals;

    for (const auto& triangle : triangles) {
        // Sommets
        file << "v " << triangle.v1[0] << " " << triangle.v1[1] << " " << triangle.v1[2] << "\n";
        file << "v " << triangle.v2[0] << " " << triangle.v2[1] << " " << triangle.v2[2] << "\n";
        file << "v " << triangle.v3[0] << " " << triangle.v3[1] << " " << triangle.v3[2] << "\n";

        // Calcul de la normale
        float ux = triangle.v2[0] - triangle.v1[0];
        float uy = triangle.v2[1] - triangle.v1[1];
        float uz = triangle.v2[2] - triangle.v1[2];

        float vx = triangle.v3[0] - triangle.v1[0];
        float vy = triangle.v3[1] - triangle.v1[1];
        float vz = triangle.v3[2] - triangle.v1[2];

        float nx = uy * vz - uz * vy;
        float ny = uz * vx - ux * vz;
        float nz = ux * vy - uy * vx;

        float length = std::sqrt(nx * nx + ny * ny + nz * nz);
        if (length > 0.00001f) {
            nx /= length;
            ny /= length;
            nz /= length;
        }

        normals.push_back({ nx, ny, nz });
    }

    // Écriture des normales
    for (const auto& n : normals) {
        file << "vn " << n[0] << " " << n[1] << " " << n[2] << "\n";
    }

    // Écriture des faces avec normales
    for (size_t i = 0; i < triangles.size(); ++i) {
        // chaque triangle a 3 sommets et une seule normale
        int v1 = 3 * i + 1;
        int v2 = 3 * i + 2;
        int v3 = 3 * i + 3;
        int nIndex = i + 1;

        file << "f "
            << v1 << "//" << nIndex << " "
            << v2 << "//" << nIndex << " "
            << v3 << "//" << nIndex << "\n";
    }

    file.close();
}