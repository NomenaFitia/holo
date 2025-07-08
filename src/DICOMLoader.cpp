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

VolumeData DICOMLoader::loadFromDirectoryWithLeap(const std::string& directoryPath, int leap) {
    if (leap <= 0) {
        throw std::invalid_argument("Leap must be a positive integer.");
    }

    std::vector<DicomSlice> allSlices;

    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".dcm") {
            DcmFileFormat file;
            if (file.loadFile(entry.path().string().c_str()).good()) {
                DcmDataset* dataset = file.getDataset();

                double sliceLocation = 0.0;
                dataset->findAndGetFloat64(DCM_SliceLocation, sliceLocation);

                DicomImage image(entry.path().string().c_str());
                if (image.getStatus() == EIS_Normal && image.isMonochrome()) {
                    const auto width = image.getWidth();
                    const auto height = image.getHeight();
                    const uint16_t* pixelData = reinterpret_cast<const uint16_t*>(image.getOutputData(16));
                    if (pixelData) {
                        std::vector<uint16_t> pixels(pixelData, pixelData + width * height);
                        allSlices.push_back({ entry.path().string(), sliceLocation, std::move(pixels) });
                    }
                }
            }
        }
    }

    if (allSlices.empty()) {
        throw std::runtime_error("No valid DICOM slices found.");
    }

    // Trier selon la position des coupes
    std::sort(allSlices.begin(), allSlices.end(), [](const DicomSlice& a, const DicomSlice& b) {
        return a.position < b.position;
        });

    // Sauter les tranches selon le leap
    std::vector<DicomSlice> slices;
    for (size_t i = 0; i < allSlices.size(); i += leap) {
        slices.push_back(std::move(allSlices[i]));
    }

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

    // Retourne le volume avec un "leap" espacement
    return { volume, width, height, depth};
}
