#include "DICOMLoader.h"  
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>
#include <filesystem>
#include <iostream>
#include <algorithm>

/*
Les données dont on a besoin pour représenter un volume 3D : 
- Pixel Spacing (0028,0030) – [X, Y] : donne l'espacement physique entre les pixels dans le plan XY. (2D) en mm
- Slice Thickness (0018,0050) : donne l'épaisseur de chaque coupe dans la direction Z. en mm
- Spacing Between Slices (0018,0088) – espacement réel entre slices si présent 
- Image Position (Patient) (0020,0032) : position de l'image dans l'espace patient, utile pour déterminer la position de chaque slice dans le volume 3D.
- Image Orientation (Patient) (0020,0037) : orientation de l'image dans l'espace patient, utile pour déterminer l'orientation des slices.
*/
namespace fs = std::filesystem;

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
    if (leap < 0) { // valider un leap 
        throw std::invalid_argument("Leap must be a positive integer.");
    }


    OFString pixelSpacingStr;
    OFString sliceThicknessStr;

    Float64 spacingX = 0.0;
    Float64 spacingY = 0.0;
	Float64 spacingZ = 0.0;


	// Image Position and Image Orientation can be helful to determine the spatial arrangement of slices
    OFString imagePositionPatientStr;
    OFString imageOrientationPatientStr;

    /*
    Dans notre cas , le slice thickness corrspond à la position Z de chaque slice.
    */

    double rescaleSlope;
	double rescaleIntercept;


    std::vector<DicomSlice> allSlices;

    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".dcm") {
            DcmFileFormat file;
            if (file.loadFile(entry.path().string().c_str()).good()) {
                DcmDataset* dataset = file.getDataset();

                
                double sliceLocation = 0.0;
                dataset->findAndGetFloat64(DCM_SliceLocation, sliceLocation);
                
				// check first file for pixel spacing and slice thickness

                if (pixelSpacingStr.empty() 
                    || sliceThicknessStr.empty()
                    || (spacingX == 0.0 && spacingY == 0.0)) {

                    dataset->findAndGetFloat64(DCM_PixelSpacing, spacingX, 0);
                    dataset->findAndGetFloat64(DCM_PixelSpacing, spacingY, 1);
                    //dataset->findAndGetOFString(DCM_SliceThickness, sliceThicknessStr);
					dataset->findAndGetFloat64(DCM_SliceThickness, spacingZ);
                }


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

    std::sort(allSlices.begin(), allSlices.end(), [](const DicomSlice& a, const DicomSlice& b) {
        return a.position < b.position;
        });

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

	// Convert pixel value in volume to Hounsfield Units (HU) before returning

    //spacingX *= static_cast<double>(leap);
    //spacingY *= static_cast<double>(leap);
    spacingZ *= static_cast<double>(leap);


    return { volume, width, height, depth, {spacingX, spacingY, spacingZ} };
}
