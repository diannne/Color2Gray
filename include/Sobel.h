/*
SOBEL FILTER USING MPI
CENTRO UNIVERSITARIO DE CIENCIAS EXACTAS E INGIENERIAS
CONCURRENT AND DISTRIBUTED PROGRAMMING 2014A


Venegas Munoz Carlos
Navarro Palos Carlos Eduardo

*/
#ifndef SOBEL_H
#define SOBEL_H

#include "Definitions.h"

namespace sobel
{

class Sobel
{
public:
    Sobel();
    ~Sobel();

    /*
    This methods apply the sobel filter given in the filter matrix to the imageData
    */
        void applySobelFilter(const int** filter,  BYTE* imageData);
protected:
private:

};

}


#endif // SOBEL_H
