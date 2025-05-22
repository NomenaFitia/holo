#pragma once
#include <vector>
#include <string>

struct VolumeData {
    std::vector<uint16_t> voxels;
    int width;
    int height;
    int depth;
};


/*

redefini
struct Volume3D {
    std::vector<uint16_t> voxels;
    int width, height, depth;

    float spacingX, spacingY, spacingZ;
    float originX, originY, originZ;
    float direction[3][3];

    inline float at(int x, int y, int z) const {
        return voxels[x + y * width + z * width * height];
    }

    inline void set(int x, int y, int z, float val) {
        voxels[x + y * width + z * width * height] = val;
    }
};

*/

class DICOMLoader {
public:
    static VolumeData loadFromDirectory(const std::string& directoryPath);
	//static Volume3D loadDicomSeries(const std::string& directoryPath);
};