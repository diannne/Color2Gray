/*
 * MPI Gooch Algorithm for Converting to Greyscale
 * Diana Popa
 * Polytechnica University of Bucharest
 */

#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

/*
You must change these values in order to change the convolution matrix size, if not, you could break
all the code because these constant are being used in a lot of parts of the code.
 */
#define CONVOLUTION_MATRIX_ROWS 3
#define CONVOLUTION_MATRIX_COLUMNS 3
/*
THis file contains all the data type definitions neccesaries for Sobel and BMP.
 */

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

const int convolutionMatrixX[CONVOLUTION_MATRIX_ROWS][CONVOLUTION_MATRIX_COLUMNS] ={
	{-1, -2, -1},
	{0, 0, 0},
	{1, 2, 1}
};

const int convolutionMatrixY[CONVOLUTION_MATRIX_ROWS][CONVOLUTION_MATRIX_COLUMNS] ={
	{-1, 0, 1},
	{-2, 0, 2},
	{-1, 0, 1}
};


#endif // DEFINITIONS_H_INCLUDED
