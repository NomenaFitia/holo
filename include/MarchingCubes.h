#pragma once
#include <DirectXMath.h>
#include <DICOMLoader.h>
#include <vector>
#include <string>

struct Triangle {
    float v1[3];
    float v2[3];
    float v3[3];
};

class MarchingCubes {
public:
    //static std::vector<Triangle> generateSurface(const VolumeData& volumeData, uint16_t isovalue);
    static void saveToObj(const std::vector<Triangle>& triangles, const std::string& filename);
    static std::vector<Triangle> generateSurface(const VolumeData& volumeData, uint16_t isovalue);
    static void saveToObjWithNormals(const std::vector<Triangle>& triangles, const std::string& filename);

private:
    static float interpolate(float val1, float val2, uint16_t isovalue, float x1, float x2);
    static uint8_t computeCubeIndex(const VolumeData& volumeData, int x, int y, int z, uint16_t isovalue);
    static void processCube(const VolumeData& volumeData, int x, int y, int z, uint16_t isovalue, const VoxelSpacing& spacing, std::vector<Triangle>& triangles);
    
    //static void processCube(const VolumeData& volumeData, int x, int y, int z, uint16_t isovalue, std::vector<Triangle>& triangles);
};