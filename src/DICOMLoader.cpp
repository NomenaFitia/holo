#include "DICOMLoader.h"  
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>
#include <filesystem>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;


// tolkony soloina liste de pixels
struct DicomSlice {
    std::string filename;
    double position;
    std::vector<uint16_t> pixels;
};

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
