/*
SOBEL FILTER USING MPI
CENTRO UNIVERSITARIO DE CIENCIAS EXACTAS E INGIENERIAS
CONCURRENT AND DISTRIBUTED PROGRAMMING 2014A


Venegas Munoz Carlos
Navarro Palos Carlos Eduardo

*/
#include <iostream>
#include "ImageManipulator.h"

using namespace imageManipulator;

/*
THis main is the example of how to convert a normal BMP image to grayscale Charly. As you see only three methods are called :D

*/
int main()
{
    ImageManipulator* imageManipulator = new ImageManipulator();
    imageManipulator->openImage("/home/cnavarropalos/example.bmp");
    imageManipulator->convertToGrayscale();
    imageManipulator->saveImage("/home/cnavarropalos/grayscale.bmp");
    return 0;
}
