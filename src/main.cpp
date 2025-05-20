#include <dcmtk/dcmdata/dctk.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <DICOMLoader.h>


int main() {

    std::string directoryPath = "C:\\Users\\MIPA\\Documents\\Projets\\dcm";

    try {
		DICOMLoader loader;
		VolumeData volume = loader.loadFromDirectory(directoryPath);
		std::cout << "Loaded volume with dimensions: " << volume.width << " x " << volume.height << " x " << volume.depth << std::endl;


		/* test pixels value
		* 
		* 
		for (size_t i = 0; i < volume.width * volume.height * volume.depth; i++)
		{
			if (volume.voxels[i] != 0) {
				std::cout << "Loaded :" << volume.voxels[i] << std::endl;
			}
		}

		*/
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	std::cout << "Finished processing DICOM files." << std::endl;
    return 0;
}