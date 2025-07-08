#include <dcmtk/dcmdata/dctk.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <DICOMLoader.h>
#include <DirectXRenderer.h>


int main() 
{
    std::string directoryPath = "C:\\Users\\MIPA\\Documents\\Projets\\dcm";
	float isovalue = 0.5f;
	int leap = 100;

    try {
		DICOMLoader loader;
		VolumeData volume = loader.loadFromDirectoryWithLeap(directoryPath, leap);

		auto triangles = MarchingCubes::generateSurface(volume, 30000);
		//MarchingCubes::saveToObj(triangles, "output.obj");
		MarchingCubes::saveToObjWithNormals(triangles, "outputWithNormals.obj");

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	std::cout << "Finished processing DICOM files." << std::endl;
    return 0;
}