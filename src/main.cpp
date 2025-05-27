#include <dcmtk/dcmdata/dctk.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <DICOMLoader.h>
#include <DirectXRenderer.h>


int main() 
{
	int width, height, depth;
    std::string directoryPath = "C:\\Users\\MIPA\\Documents\\Projets\\dcm";
	float isovalue = 0.5f;

    try {
		DICOMLoader loader;
		VolumeData volume = loader.loadFromDirectory(directoryPath);

		auto triangles = MarchingCubes::generateSurface(volume, 30000); // confirmation de la valeur d'iso pour le squlette 
		//MarchingCubes::saveToObj(triangles, "output.obj");

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	std::cout << "Finished processing DICOM files." << std::endl;
    return 0;
}