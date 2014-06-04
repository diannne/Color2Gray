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
int main( int argc, char *argv[])
{
    if ( argc != 2 )
    {
        std::cout<<"Please give me a file to use" std::endl;
	return -1 ;
    }
    ImageManipulator* imageManipulator = new ImageManipulator();
    imageManipulator->openImage(argv[1]);
    imageManipulator->convertToGrayscale();
    imageManipulator->saveImage(argv[1]);
    return 0;
}
