#pragma once
#include <vector>
#include <string>

// à modifier 
struct VolumeData {
    std::vector<uint16_t> voxels; // 3D volume in 1D layout: x + y*width + z*width*height
    int width;
    int height;
    int depth;
};


class DICOMLoader {
public:
    static VolumeData loadFromDirectory(const std::string& directoryPath);
};