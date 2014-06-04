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

Sobel::Sobel()
{

}

Sobel::~Sobel()
{

}

void Sobel::applySobelFilter(const int filter[CONVOLUTION_MATRIX_ROWS][CONVOLUTION_MATRIX_COLUMNS],  const BYTE* imageData)
{
    /*
    THIS CODE IS ONLY FOR TESTING!!!
    */
    for(int i = 0; i < CONVOLUTION_MATRIX_ROWS; i++)
    {
        for(int j = 0; j < CONVOLUTION_MATRIX_COLUMNS; j++)
        {
            cout << filter[i][j] << " ";

        }
        cout << endl;
    }



}
