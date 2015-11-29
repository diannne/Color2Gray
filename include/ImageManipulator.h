/*
SOBEL FILTER USING MPI
CENTRO UNIVERSITARIO DE CIENCIAS EXACTAS E INGIENERIAS
CONCURRENT AND DISTRIBUTED PROGRAMMING 2014A


Venegas Munoz Carlos
Navarro Palos Carlos Eduardo

 */

#ifndef IMAGEMANIPULATOR_H
#define IMAGEMANIPULATOR_H

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>

#include "Definitions.h"
#include "Color_Space_Conversions.h"

#define BITS_PER_PIXEL 24
#define RGB2GRAY(r,g,b) (BYTE)( double(0.299) * r + double(0.587) * g + double(0.114) * b + 0.5)
#define CLAMP(x)  (((x) > (255)) ? (255) : (((x) < (0)) ? (0) : (x)))

namespace imageManipulator {

	/*
	This struct contains the all the data from the header of a BMP Image.
	You can check these documantation in: http://es.wikipedia.org/wiki/Windows_bitmap
	 */
	struct BMPHeader {
		WORD identifier;
		DWORD size;
		DWORD reserved;
		DWORD offset;
		DWORD headerSize;
		DWORD height;
		DWORD width;
		WORD planes;
		WORD bitsPerPixel;
		DWORD compression;
		DWORD imageSize;
		DWORD horizontalResolution;
		DWORD verticalResolution;
		DWORD numberOfColours;
		DWORD numberOfImportantColours;
	};

	/*
	THis class allow you to manipulate BMP images as matrix. Ypu can save
	 * the header of the BMP and use it
	for example to change the color mode, image size, etc.

	You must first call openImage(const char* filePath) in order to turn on the
	 * flag "imageLoaded" and then you can
	manipulate the image.

	IMPORTANT: This class only works with BMP with 24 bits per pixel.
	 */
	class ImageManipulator {
	public:


		ImageManipulator();
		~ImageManipulator();
		ImageManipulator(const ImageManipulator& src);
		/*
		This method reads the file and checks that the file correspond
		to an BMP and saves all the headers from this image.
		 */
		bool openImage(const char* filePath);
		/*
		This method save the current header and the current imageDate in the file system
		using the file path.
		 */
		void saveImage(const char* filePath, bool grey = false);

		/*
		This method convert all the pixels of the image in a grayscale.
		 */
		void convertToGrayscale(const int, const int, const int, const int, int &,
				int gooch = 0);

		void convertTolab(const int, const int, const int, const int);
		void computeTargetDifferences();
		double computeDelta(int, int);
		double crunch(double);
		void resolveOptimizationProblem();
		void convertToRgb(const int, const int, const int, const int);
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
		void printHeader();
		/*
		This method applies sobel filter to the image data in the given sector of the image.
		This is usefull for distributing the job.
		 */
		void applySobelsFilter(const int initialX, const int initialY,
				const int finalX, const int finalY, int &rv);
		/*
		Prints the image matrix following this form
		(r,g,b)(r,g,b)...(r,g,b)
		(r,g,b)(r,g,b)...(r,g,b)
		...
		(r,g,b)(r,g,b)...(r,g,b)

		Since an image has a huge amount of pixels, for default only shows a matrix of 10x10

		 */
		void printDataImage(const bool isTest = true, const int maxTestSize = 10);

		//protected:
		//private:

		FILE* imageFile; //The file where you load the image
		BMPHeader header; //The header of the BMP
		BYTE* imageData; //The image as a binary
		BYTE* greyImageData; //The image as a binary
		bool imageLoaded; //This flag allows you to use all the methods of this class.
		bool isGrayscale; //True if the greyscale filter was applied to the image
		CIE_Lab * _data;
		double *_deltas;
		int _radius;
		double _theta;
		double _alpha;
		double * _dataOutput;
		int _iterations;
		Color *_greyOutput;
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
