/*
SOBEL FILTER USING MPI
CENTRO UNIVERSITARIO DE CIENCIAS EXACTAS E INGIENERIAS
CONCURRENT AND DISTRIBUTED PROGRAMMING 2014A


Venegas Munoz Carlos
Navarro Palos Carlos Eduardo

*/
#include <mpi/mpi.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "ImageManipulator.h"
#include "Definitions.h"

#define NAME_LENGHT 11

using namespace imageManipulator;
using namespace std;

/*
THis main is the example of how to convert a normal BMP image to grayscale Charly. As you see only three methods are called :D

*/
int main( int argc, char *argv[])
{
    if ( argc != 2 )
    {
        cout << "Please give me a file to use" << endl;
        return -1;
    }

    ifstream hostsFile("hosts");
    string line;
    int numberHosts = 0;
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

    string fileName(argv[1]);
    string grayscaleFileName = fileName;
    string sobelFileName = fileName;

    int worldSize;
    int worldRank;
    char processorName[MPI_MAX_PROCESSOR_NAME];
    int nameLength;

    grayscaleFileName.insert(fileName.size() - 4, "_grayscale");
    sobelFileName.insert(fileName.size() - 4, "_sobel");

    ImageManipulator* imageManipulator = new ImageManipulator();
    imageManipulator->openImage(fileName.c_str());


    imageManipulator->convertToGrayscale();
    imageManipulator->saveImage(grayscaleFileName.c_str());


    int sectionHeight = imageManipulator->header.height / numberHosts;
    int sectionHeightBuffer = 0;

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    // Get the name of the processor
    MPI_Get_processor_name(processorName, &nameLength);

    for(int i = 0; i < numberHosts; i++)
    {
        string section1FileName = fileName;
        ImageManipulator imageSection;
        char sectionName[NAME_LENGHT];
        snprintf(sectionName, sizeof(sectionName), "%d", i);
        section1FileName.insert(fileName.size() - 4, sectionName);

        imageSection = imageManipulator->applySobelsFilter(0, sectionHeightBuffer, imageManipulator->header.widht, sectionHeightBuffer + sectionHeight);
        imageSection.saveImage(section1FileName.c_str());

        sectionHeightBuffer += sectionHeight;

        cout << "Filter applied from processor " << processorName << ", rank " << worldRank << " out of " << worldSize << " processor" << endl;

        //should be worldSize and not numberHosts
        //cout << "Filter applied from processor " << processorName << ", rank " << i + 1 << " out of " << numberHosts << " processor" << endl;
    }

    //Only the clean up the borders. THis call is optional
    imageManipulator->applyBrihtness(50);
    imageManipulator->saveImage(sobelFileName.c_str());

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}
