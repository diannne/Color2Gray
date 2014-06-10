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
    BYTE gradient = 0;
    for(int y = initialY; y < finalY; y++)
    {
        for(int x = initialX; x < finalX; x++)
        {
            Color pixel = edgeDetectedImage.getPixel(x, y);

            /**
            These gx and gy should be retrieved using the convulotion matrix from definitions.h but this is a fastest way to calculate the gradient.
            **/
            float gx = (((2 * edgeDetectedImage.getPixel(x + 2, y + 2).b) + edgeDetectedImage.getPixel(x + 2, y).b + edgeDetectedImage.getPixel(x + 2, y + 2).b) -
                        (2 * edgeDetectedImage.getPixel(x, y + 1).b + edgeDetectedImage.getPixel(x, y).b + edgeDetectedImage.getPixel(x, y + 2).b));

            float gy = (((2 * edgeDetectedImage.getPixel(x + 1, y + 2).b) + edgeDetectedImage.getPixel(x, y + 2).b + edgeDetectedImage.getPixel(x + 2, y + 2).b) -
                        (2 * edgeDetectedImage.getPixel(x + 1, y).b + edgeDetectedImage.getPixel(x, y).b + edgeDetectedImage.getPixel(x + 2, y).b));


            gradient = (BYTE)round(sqrt(pow(gx, 2) + pow(gy, 2)));

            pixel.r = gradient;
            pixel.g = gradient;
            pixel.b = gradient;

            edgeDetectedImage.setPixel(x, y, pixel);
        }
    }
}
