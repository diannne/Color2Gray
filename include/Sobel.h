/*
SOBEL FILTER USING MPI
CENTRO UNIVERSITARIO DE CIENCIAS EXACTAS E INGIENERIAS
CONCURRENT AND DISTRIBUTED PROGRAMMING 2014A


Venegas Munoz Carlos
Navarro Palos Carlos Eduardo

*/
#ifndef SOBEL_H
#define SOBEL_H

#include <iostream>
#include <math.h>
#include "Definitions.h"
#include "ImageManipulator.h"

namespace sobel
{

class Sobel
{
public:
    Sobel(imageManipulator::ImageManipulator originalImage);
    ~Sobel();
    /*
    This method applies the sobel filter in the sector of the image given in the parameters.
    */
    void applySobelFilter(const int initialX, const int initialY, const int finalX, const int finalY);


//protected:
//private:
    imageManipulator::ImageManipulator originalImage;
    imageManipulator::ImageManipulator edgeDetectedImage;







};

}


#endif // SOBEL_H
