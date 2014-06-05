/*
SOBEL FILTER USING MPI
CENTRO UNIVERSITARIO DE CIENCIAS EXACTAS E INGIENERIAS
CONCURRENT AND DISTRIBUTED PROGRAMMING 2014A


Venegas Munoz Carlos
Navarro Palos Carlos Eduardo

*/
#include "Sobel.h"

using namespace sobel;
using namespace std;

Sobel::Sobel(imageManipulator::ImageManipulator originalImage)
{
    this->originalImage = originalImage;

}

Sobel::~Sobel()
{

}

void Sobel::applySobelFilter(const int initialX, const int initialY, const int finalX, const int finalY)
{

}
