/*
 * MPI Gooch Algorithm for Converting to Greyscale
 * Diana Popa
 * Polytechnica University of Bucharest
*/

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "ImageManipulator.h"
#include "Definitions.h"

#define NAME_LENGTH 11

using namespace imageManipulator;
using namespace std;

int rv;

int main( int argc, char *argv[])
{
    if ( argc != 2 )
    {
        cout << "Usage: mpirun -np [task#] ./exec bmp_image" << endl;
        return -1;
    }

    ifstream hostsFile("hosts");
    string line;
    int numberHosts = 0;
    int worldSize;
    int rank;
    char processorName[MPI_MAX_PROCESSOR_NAME];
    int nameLength;

    if(hostsFile)
    {
        while (getline(hostsFile, line))
        {
            numberHosts++;
        }
    }
    else
    {
        cout << "Could not read hosts file." << endl;
        return -1;
    }

	rv = MPI_Init(&argc,&argv);
	
	if( rv != MPI_SUCCESS )
	{
		cout<<"Error Initializing MPI Environment";
		MPI_Abort(MPI_COMM_WORLD, rv);
		return -1;
	}
    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Get the name of the processor
    MPI_Get_processor_name(processorName, &nameLength);

    string fileName(argv[1]);
    string sobelFileName = fileName;
	
    sobelFileName.insert(fileName.size() - 4, "_sobel");
	ImageManipulator imageManipulator;
    imageManipulator.openImage(fileName.c_str());
	
	string sectionFileName = fileName;
	char sectionName[NAME_LENGTH];
	snprintf(sectionName, sizeof(sectionName), "%d", rank);
	sectionFileName.insert(fileName.size() - 4, sectionName);
	
	printf("Image width: %d \nImage Height: %d\n", imageManipulator.getWidth(),
		imageManipulator.getHeight());
	
	int sectionHeight = imageManipulator.getHeight() / worldSize;
    bool gooch = false;
	ImageManipulator littleIm(imageManipulator);
	littleIm.setHeight((DWORD)sectionHeight);
    littleIm.convertToGrayscale(0, 
		rank * sectionHeight, littleIm.getWidth(), 
		((rank + 1)*sectionHeight), rv, gooch);
    
	if ( rv  ){
		cout << "Conversion to basic grayscale failed for " << rank << "th processor" << endl;
		MPI_Finalize();
		return -1;
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Gather(littleIm.greyImageData + sectionHeight*rank*littleIm.getWidth()*3, 
	sectionHeight*littleIm.getWidth()*3, MPI_CHAR, imageManipulator.greyImageData, 
	sectionHeight*littleIm.getWidth()*3, MPI_CHAR, 0, MPI_COMM_WORLD);
	
	if ( rank == 0 )
	{
		string grayscaleFileName = fileName;
		grayscaleFileName.insert(fileName.size() - 4, "_basic_grayscale");
		imageManipulator.saveImage(grayscaleFileName.c_str(), true);
	}
	
	gooch = true;
	littleIm.convertToGrayscale(0, 
		rank * sectionHeight, littleIm.getWidth(), 
		((rank + 1)*sectionHeight), rv, gooch);
	
	if ( rv  ){
		cout << "Conversion to gooch grayscale failed for " << rank << "th processor" << endl;
		MPI_Finalize();
		return -1;
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Gather(littleIm.greyImageData + sectionHeight*rank*littleIm.getWidth()*3, 
	sectionHeight*littleIm.getWidth()*3, MPI_CHAR, imageManipulator.greyImageData, 
	sectionHeight*littleIm.getWidth()*3, MPI_CHAR, 0, MPI_COMM_WORLD);
	
	if ( rank == 0 )
	{
		string grayscaleFileName = fileName;
		grayscaleFileName.insert(fileName.size() - 4, "_gooch_grayscale");
		imageManipulator.saveImage(grayscaleFileName.c_str(), true);
	}
	
	
	littleIm.applySobelsFilter(0, 
		rank * sectionHeight, littleIm.getWidth(), 
		((rank + 1)*sectionHeight), rv);
	
	if ( rv ){
		cout << "Applying Sobel Filter failed for " << rank << "th processor" << endl;
		MPI_Finalize();
		return -1;
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Gather(littleIm.imageData + sectionHeight*rank*littleIm.getWidth()*3, 
	sectionHeight*littleIm.getWidth()*3, MPI_CHAR, imageManipulator.imageData, 
	sectionHeight*littleIm.getWidth()*3, MPI_CHAR, 0, MPI_COMM_WORLD);
	
	if ( rank == 0 )
	{
		string sobelImageFile = fileName;
		sobelImageFile.insert(fileName.size() - 4, "_sobel");
		imageManipulator.saveImage(sobelImageFile.c_str());
	}
	
	//imageSection.saveImage(sectionFileName.c_str());

	//sectionHeightBuffer += sectionHeight;

	       
    //Only the clean up the borders. This call is optional
    //imageManipulator.applyBrightness(50);
    //imageManipulator.saveImage(sobelFileName.c_str());

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}
