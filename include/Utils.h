#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <DICOMLoader.h>

const int CAT_UNKNOWN = 0;
const int CAT_AIR = 1;
const int CAT_LUNG = 2;
const int CAT_FAT = 3;
const int CAT_SOFT_TISSUE = 4;
const int CAT_BONE = 5;
const int CAT_METAL = 6;

static int voxelToHU(int voxelValue, double rescaleSlope, double rescaleIntercept);
static int categorizeHU(int hu);
static std::string categoryToString(int category);

static void categorizeVolume(VolumeData& volume, double rescaleSlope, double rescaleIntercept );

#endif
