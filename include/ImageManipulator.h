/*
 * MPI Gooch Algorithm for Converting to Greyscale
 * Diana Popa
 * Polytechnica University of Bucharest
 */


#ifndef IMAGEMANIPULATOR_H
#define IMAGEMANIPULATOR_H

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <string>
#include "Definitions.h"
#include "Color_Space_Conversions.h"
#include "bitmap_image.hpp"

#define BITS_PER_PIXEL 24
#define RGB2GRAY(r,g,b) (BYTE)( double(0.299) * r + double(0.587) * g + double(0.114) * b + 0.5)
#define CLAMP(x)  (((x) > (255)) ? (255) : (((x) < (0)) ? (0) : (x)))

namespace imageManipulator {

	class ImageManipulator {
	public:

		ImageManipulator();
		~ImageManipulator();
		ImageManipulator(const ImageManipulator& src);
		
		bool openImage(bitmap_image );
		void saveImage(std::string, bitmap_image);
		
		
		/*
		This method convert all the pixels of the image in a grayscale.
		 */
		void convertToGrayscale(const int, const int, const int, const int, int gooch = 0);

		void convertTolab(const int, const int, const int, const int);
		void computeTargetDifferences();
		double computeDelta(int, int);
		double crunch(double);
		void resolveOptimizationProblem();
		void convertToRgb(const int, const int, const int, const int, int);
		void SetGreyOutput(const int, const int, const int, const int);
		void writeLabData();
		void readLabData();
		void writeDeltasData();
		void readDeltasData();

		void writeOptimizedData();
		void readOptimizedData();

		/*
		This method changes the brightness of the entire image.
		 */
		void applyBrightness(const int level);
		/*
		This method prints the header INFO
		 */

		void printDataImage();

		
		
		FILE* imageFile; //The file where you load the image
		BYTE* imageData; //The image as a binary
		BYTE* greyImageData; //The image as a binary
		int _width;
		int _height;
		
		bool isGrayscale;
		CIE_Lab * _data;
		double *_deltas;
		int _radius;
		double _theta;
		double _alpha;
		double * _dataOutput;
		int _iterations;
		Color *_greyOutput;
		double _sinTheta;
		double _cosTheta;
		/*
		This method returns a Color struct using the given x and y coordinates. The color struct
		contains the pixel in RGB form. The programmer is responsable to check that the indexes are a valids
		indexes.
		 */
		Color getPixel(BYTE* imageData, const int x, const int y);
		/*
		This method put the the new pixel with the given color in the given coordinates.
		The programmer is responsable to check that the indexes are a valids
		indexes.
		 */
		void setPixel(BYTE* imageData, const int x, const int y, const Color color);

		DWORD getHeight();
		DWORD getWidth();

		void setHeight(DWORD);


	};

}


#endif // IMAGEMANIPULATOR_H
