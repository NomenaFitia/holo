#include <Utils.h>


int voxelToHU(int voxelValue, double rescaleSlope, double rescaleIntercept) {
    return static_cast<int>(voxelValue * rescaleSlope + rescaleIntercept);
}

int categorizeHU(int hu) {
    if (hu < -900) return CAT_AIR;
    else if (hu >= -900 && hu < -500) return CAT_LUNG;
    else if (hu >= -500 && hu < -100) return CAT_FAT;
    else if (hu >= -100 && hu < 300) return CAT_SOFT_TISSUE;
    else if (hu >= 300 && hu < 3000) return CAT_BONE;
    else if (hu >= 3000) return CAT_METAL;
    else return CAT_UNKNOWN;
}

std::string categoryToString(int category) {
    switch (category) {
    case CAT_AIR: return "Air";
    case CAT_LUNG: return "Lung";
    case CAT_FAT: return "Fat";
    case CAT_SOFT_TISSUE: return "Soft Tissue";
    case CAT_BONE: return "Bone";
    case CAT_METAL: return "Metal";
    default: return "Unknown";
    }
}

void categorizeVolume(VolumeData& volume, const double rescaleSlope, const double rescaleIntercept)
{
    int hu;
    uint16_t category = 0;

    for (auto& voxel : volume.voxels) {
        hu = voxelToHU(voxel, rescaleSlope, rescaleIntercept);
        voxel = categorizeHU(hu); // Store category instead of HU value
    }
}
