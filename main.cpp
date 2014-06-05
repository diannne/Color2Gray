/*
SOBEL FILTER USING MPI
CENTRO UNIVERSITARIO DE CIENCIAS EXACTAS E INGIENERIAS
CONCURRENT AND DISTRIBUTED PROGRAMMING 2014A


Venegas Munoz Carlos
Navarro Palos Carlos Eduardo

*/
#include <iostream>
#include "ImageManipulator.h"
#include "Definitions.h"

using namespace imageManipulator;
using namespace std;

/*
THis main is the example of how to convert a normal BMP image to grayscale Charly. As you see only three methods are called :D

*/
int main()
{
    int numberOfHosts = 4;
    int imageHeight;
    int imageWidht;
    BYTE* section1;
    BYTE* section2;
    BYTE* section3;
    ImageManipulator* imageManipulator = new ImageManipulator();
    imageManipulator->openImage("/home/cnavarropalos/example.bmp");

    //imageHeight = imageManipulator->header.height; //465
    //imageWidht = imageManipulator->header.widht; //620

    imageManipulator->convertToGrayscale();
    imageManipulator->saveImage("/home/cnavarropalos/grayscale.bmp");

    section1 = imageManipulator->applySobelsFilter(0, 100, 0, 100);
    section2 = imageManipulator->applySobelsFilter(101, 200, 101, 200);
    section3 = imageManipulator->applySobelsFilter(201, 465, 201, 620);

    imageManipulator->saveImage("/home/cnavarropalos/sobel.bmp");
    return 0;
}
