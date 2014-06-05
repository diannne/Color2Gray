/*
SOBEL FILTER USING MPI
CENTRO UNIVERSITARIO DE CIENCIAS EXACTAS E INGIENERIAS
CONCURRENT AND DISTRIBUTED PROGRAMMING 2014A


Venegas Munoz Carlos
Navarro Palos Carlos Eduardo

*/
#include "Sobel.h"

using namespace sobel;
using namespace imageManipulator;
using namespace std;

Sobel::Sobel(imageManipulator::ImageManipulator originalImage)
{
    this->originalImage = originalImage;
    this->originalImage.imageLoaded = true;
    edgeDetectedImage = originalImage;
    edgeDetectedImage.imageLoaded = true;
}

Sobel::~Sobel()
{

}

void Sobel::applySobelFilter(const int initialX, const int initialY, const int finalX, const int finalY)
{
    for(int y = initialY; y < finalY; y++)
    {
        for(int x = initialX; x < finalX; x++)
        {
            Color color = edgeDetectedImage.getPixel(x, y);

            color.r = 0;
            color.g = 0;
            color.b = 0;

            edgeDetectedImage.setPixel(x, y, color);
        }
    }

}
