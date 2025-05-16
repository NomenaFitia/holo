#include "DICOMLoader.h"  
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>
#include <filesystem>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

struct DicomSlice {
    std::string filename;
    double position;
    std::vector<uint16_t> pixels;
};

struct SliceInfo {
    std::string path;
    float positionZ;
};

bool compareSlice(const SliceInfo& a, const SliceInfo& b) {
    return a.positionZ < b.positionZ;
}

VolumeData DICOMLoader::loadFromDirectory(const std::string& directoryPath) {
    std::vector<DicomSlice> slices;

    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".dcm") {
            DcmFileFormat file;
            if (file.loadFile(entry.path().string().c_str()).good()) {
                DcmDataset* dataset = file.getDataset();

                double sliceLocation = 0.0;
                dataset->findAndGetFloat64(DCM_SliceLocation, sliceLocation);

                DicomImage image(entry.path().string().c_str());
                if (image.getStatus() == EIS_Normal) {
                    if (image.isMonochrome()) {
                        const auto width = image.getWidth();
                        const auto height = image.getHeight();
                        const uint16_t* pixelData = reinterpret_cast<const uint16_t*>(image.getOutputData(16));
                        if (pixelData) {
                            std::vector<uint16_t> pixels(pixelData, pixelData + width * height);
                            slices.push_back({ entry.path().string(), sliceLocation, std::move(pixels) });
                        }
                    }
                }

				// Get the slice location
            }
        }
    }

    if (slices.empty()) {
        throw std::runtime_error("No valid DICOM slices found.");
    }

    std::sort(slices.begin(), slices.end(), [](const DicomSlice& a, const DicomSlice& b) {
        return a.position < b.position;
     });

    int width = DicomImage(slices[0].filename.c_str()).getWidth();
    int height = DicomImage(slices[0].filename.c_str()).getHeight();
    int depth = static_cast<int>(slices.size());

    std::vector<uint16_t> volume(width * height * depth);

    for (int z = 0; z < depth; ++z) {
        const auto& slice = slices[z];
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                volume[x + y * width + z * width * height] = slice.pixels[x + y * width];
            }
        }
    }

    return { volume, width, height, depth };
}

/*


Volume3D DICOMLoader::loadDicomSeries(const std::string& directoryPath)
{
    std::vector<SliceInfo> slices;

    // 1. Lire tous les fichiers
    for (const auto& file : fs::directory_iterator(directoryPath)) {
        DcmFileFormat fileFormat;
        if (fileFormat.loadFile(file.path().c_str()).good()) {
            DcmDataset* dataset = fileFormat.getDataset();

            Float64 imagePosition[3] = { 0 };
            dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[0], 0);
            dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[1], 1);
            dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[2], 2);

            slices.push_back({ file.path().string(), static_cast<float>(imagePosition[2]) });
        }
    }

    // 2. Trier les slices par position Z
    std::sort(slices.begin(), slices.end(), compareSlice);

    // 3. Lire la première image pour extraire la géométrie
    DcmFileFormat fileFormat;
    fileFormat.loadFile(slices[0].path.c_str());
    DcmDataset* dataset = fileFormat.getDataset();

    Uint16 rows, cols;
    dataset->findAndGetUint16(DCM_Rows, rows);
    dataset->findAndGetUint16(DCM_Columns, cols);

    Float64 spacing[2];
    dataset->findAndGetFloat64(DCM_PixelSpacing, spacing[0], 0);
    dataset->findAndGetFloat64(DCM_PixelSpacing, spacing[1], 1);

    Float64 orientation[6];
    for (int i = 0; i < 6; ++i)
        dataset->findAndGetFloat64(DCM_ImageOrientationPatient, orientation[i], i);

    Float64 origin[3];
    for (int i = 0; i < 3; ++i)
        dataset->findAndGetFloat64(DCM_ImagePositionPatient, origin[i], i);

    // 4. Allouer le volume
    Volume3D vol;
    vol.width = cols;
    vol.height = rows;
    vol.depth = slices.size();
    vol.spacingX = spacing[0];
    vol.spacingY = spacing[1];
    vol.spacingZ = std::abs(slices[1].positionZ - slices[0].positionZ);
    vol.originX = origin[0];
    vol.originY = origin[1];
    vol.originZ = origin[2];

    vol.direction[0][0] = orientation[0]; // X row
    vol.direction[0][1] = orientation[1];
    vol.direction[0][2] = orientation[2];
    vol.direction[1][0] = orientation[3]; // Y row
    vol.direction[1][1] = orientation[4];
    vol.direction[1][2] = orientation[5];

    // Z axis = cross product of row & column direction
    vol.direction[2][0] = orientation[1] * orientation[5] - orientation[2] * orientation[4];
    vol.direction[2][1] = orientation[2] * orientation[3] - orientation[0] * orientation[5];
    vol.direction[2][2] = orientation[0] * orientation[4] - orientation[1] * orientation[3];

    vol.voxels.resize(cols * rows * vol.depth);

    // 5. Charger les pixels slice par slice
    for (int z = 0; z < vol.depth; ++z) {
        DcmFileFormat ff;
        ff.loadFile(slices[z].path.c_str());
        DcmDataset* ds = ff.getDataset();

        const Uint16* pixelData = nullptr;
        ds->findAndGetUint16Array(DCM_PixelData, pixelData);

        for (int y = 0; y < rows; ++y)
            for (int x = 0; x < cols; ++x) {
                int idx = x + y * cols + z * cols * rows;
                vol.voxels[idx] = static_cast<float>(pixelData[x + y * cols]);
            }
    }

    std::cout << "Loaded DICOM series with dimensions: "
        << vol.width << " x " << vol.height << " x " << vol.depth << std::endl;
    return vol;
}

*/