#include <dcmtk/dcmdata/dctk.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <DICOMLoader.h>
#include <MarchingCubes.h>

/*
Vérifier les dimenstions 
Choisir une unité de mmesure et faire correspondre les coordonnées des points
appliquer une réduction du nombre de voxels en adaptant les dimensions
Créer une struct pour les dimensions réelles des voxels : utiliser dcmtk pour obtenir les tags
*/

int main() 
{
    std::string directoryPath = "C:\\projects\\dcmfiles";
	float isovalue = 0.5f;
	int leap = 10;
	//VoxelSpacing spacing;


	/*
	
	Créer le volume 

	Segmenter le volume 

	Créer les surfaces 

	Raffiner la surface

	Exporter la surface en .obj

	*/
    try {
		DICOMLoader loader;
		VolumeData volume = loader.loadFromDirectoryWithLeap(directoryPath, leap); // diviser pré-traitement et génération de la surface

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