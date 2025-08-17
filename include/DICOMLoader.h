#pragma once
#include <vector>
#include <string>
#include <dcmtk/ofstd/ofstring.h>

struct DicomSlice {
    std::string filename;
    double position;
    std::vector<uint16_t> pixels;
};

struct SliceInfo {
    std::string path;
    float positionZ;
};

struct VoxelSpacing {
    double x;
    double y;
    double z;
};

struct VolumeData {
	std::vector<uint16_t> voxels;
    int width;
    int height;
    int depth;
	VoxelSpacing spacing;
};

class DICOMLoader {
public:
    static VolumeData loadFromDirectory(const std::string& directoryPath);
    static VolumeData loadFromDirectoryWithLeap(const std::string& directoryPath, const int leap);
};