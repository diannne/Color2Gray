/*
SOBEL FILTER USING MPI
CENTRO UNIVERSITARIO DE CIENCIAS EXACTAS E INGIENERIAS
CONCURRENT AND DISTRIBUTED PROGRAMMING 2014A


Venegas Munoz Carlos
Navarro Palos Carlos Eduardo

*/
#include <iostream>
#include <string>
#include "ImageManipulator.h"
#include "Definitions.h"

using namespace imageManipulator;
using namespace std;

/*
THis main is the example of how to convert a normal BMP image to grayscale Charly. As you see only three methods are called :D

*/
int main( int argc, char *argv[])
{
    if ( argc != 2 )
    {
        std::cout << "Please give me a file to use" << std::endl;
        return -1 ;
    }

    std::string fileName(argv[1]);
    std::string grayscaleFileName = fileName;
    std::string sobelFileName = fileName;
    std::string section1FileName = fileName;
    std::string section2FileName = fileName;
    std::string section3FileName = fileName;
    grayscaleFileName.insert(fileName.size() - 4, "_grayscale");
    sobelFileName.insert(fileName.size() - 4, "_sobel");
    section1FileName.insert(fileName.size() - 4, "_section1");
    section2FileName.insert(fileName.size() - 4, "_section2");
    section3FileName.insert(fileName.size() - 4, "_section3");

    ImageManipulator section1;
    ImageManipulator section2;
    ImageManipulator section3;

    ImageManipulator* imageManipulator = new ImageManipulator();
    imageManipulator->openImage(fileName.c_str());


    imageManipulator->convertToGrayscale();

    imageManipulator->saveImage(grayscaleFileName.c_str());

    section1 = imageManipulator->applySobelsFilter(0, 0, 100, 100);
    section1.saveImage(section1FileName.c_str());
    section2 = imageManipulator->applySobelsFilter(101, 101, 200, 200);
    section1.saveImage(section2FileName.c_str());
    section3 = imageManipulator->applySobelsFilter(201, 201, 400, 400);
    section1.saveImage(section3FileName.c_str());

    imageManipulator->saveImage(sobelFileName.c_str());

    return 0;
}
