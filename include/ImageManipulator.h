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

#define BITS_PER_PIXEL 24
#define RGB2GRAY(r,g,b) (BYTE)( (b)*0.3 + (g)*0.59 + (r)*0.11 )

namespace imageManipulator
{

/*
This struct contains the all the data from the header of a BMP Image.
You can check these documantation in: http://es.wikipedia.org/wiki/Windows_bitmap
*/
struct BMPHeader
{

    WORD identifier;
    DWORD size;
    DWORD reserved;
    DWORD offset;
    DWORD headerSize;
    DWORD height;
    DWORD widht;
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
This struct saves the value of each pixel on the image.
*/
struct Color
{

    BYTE r;
    BYTE g;
    BYTE b;
};

/*
THis class allow you to manipulate BMP images as matrix. Ypu can save the header of the BMP and use it
for example to change the color mode, image size, etc.

You must first call openImage(const char* filePath) in order to turn on the flag "imageLoaded" and then you can
manipulate the image.

IMPORTANT: This class only works with BMP with 24 bits per pixel.
*/
class ImageManipulator
{
public:


    ImageManipulator();
    ~ImageManipulator();
    /*
    This method reads the file and checks that the file correspond
    to an BMP and saves all the headers from this image.
    */
    bool openImage(const char* filePath);
    /*
    This method save the current header and the current imageDate in the file system
    using the file path.
    */
    void saveImage(const char* filePath);
    /*
    This method convert all the pixels of the image in a grayscale.
    */
    void convertToGrayscale();
    /*
    This method prints the header INFO
    */
    void printHeader();
    /*
    This method applies sobel filter to the image data
    */
    void applySobelsFilter();

protected:
private:

    FILE* imageFile; //The file where you load the image
    BMPHeader header; //The header of the BMP
        BYTE* imageData; //The image as a binary
    bool imageLoaded; //This flag allows you to use all the methods of this class.

    /*
    This method returns a Color struct using the given x and y coordinates. The color struct
    contains the pixel in RGB form.
    */
    Color getPixel(const int x, const int y);
    /*
    This method put the the new pixel with the given color in the given coordinates.
    */
    void setPixel(const int x, const int y, const Color color);

};

}


#endif // IMAGEMANIPULATOR_H
