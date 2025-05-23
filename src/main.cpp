#include <dcmtk/dcmdata/dctk.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <DICOMLoader.h>
#include <DirectXRenderer.h>


int main() {
	int width, height, depth;
    std::string directoryPath = "C:\\Users\\MIPA\\Documents\\Projets\\dcm";
	float isovalue = 0.5f;

    try {
		DICOMLoader loader;
		VolumeData volume = loader.loadFromDirectory(directoryPath);

		/*
		* 
		* 
		std::cout << "Loaded volume with dimensions: " << volume.width << " x " << volume.height << " x " << volume.depth << std::endl;

		for (size_t i = 0; i < volume.width * volume.height * volume.depth; i++)
		{ 
			if (volume.voxels[i] > 65535) {
				std::cout << "Loaded :" << volume.voxels[i] << std::endl;
			}
		}

		*/

		auto triangles = MarchingCubes::generateSurface(volume, 30000); // confirmation de la valeur d'iso pour le squlette 

		// Sauvegarder en OBJ
		MarchingCubes::saveToObj(triangles, "output.obj");

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	std::cout << "Finished processing DICOM files." << std::endl;
    return 0;
}