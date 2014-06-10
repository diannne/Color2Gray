/*
SOBEL FILTER USING MPI
CENTRO UNIVERSITARIO DE CIENCIAS EXACTAS E INGIENERIAS
CONCURRENT AND DISTRIBUTED PROGRAMMING 2014A


Venegas Munoz Carlos
Navarro Palos Carlos Eduardo

*/
#include "ImageManipulator.h"
#include "Sobel.h"

using namespace imageManipulator;
using namespace sobel;
using namespace std;

ImageManipulator::ImageManipulator()
{
    imageData = NULL;
    imageLoaded = false;
    isGrayscale = false;
}

ImageManipulator::~ImageManipulator()
{
}

bool ImageManipulator::openImage(const char* filePath)
{
    imageData = NULL;
    imageLoaded = false;

    imageFile = fopen(filePath, "rb");
    if(imageFile)
    {

        fread(&header.identifier, 1, sizeof(WORD),  imageFile);
        fread(&header.size, 1, sizeof(DWORD), imageFile);
        fread(&header.reserved, 1, sizeof(DWORD), imageFile);
        fread(&header.offset, 1, sizeof(DWORD), imageFile);
        fread(&header.headerSize, 1, sizeof(DWORD), imageFile);
        fread(&header.widht, 1, sizeof(DWORD), imageFile);
        fread(&header.height, 1, sizeof(DWORD), imageFile);
        fread(&header.planes, 1, sizeof(WORD),  imageFile);
        fread(&header.bitsPerPixel, 1, sizeof(WORD),  imageFile);

        if(header.bitsPerPixel != BITS_PER_PIXEL)
        {
            cout << "The image must have " << BITS_PER_PIXEL << " bits per pixel and this image has " << header.bitsPerPixel << " bits per pixel. Try with another image."<< endl;
            return false;
        }
        fread(&header.compression, 1, sizeof(DWORD), imageFile);
        fread(&header.imageSize, 1, sizeof(DWORD), imageFile);
        fread(&header.horizontalResolution, 1, sizeof(DWORD), imageFile);
        fread(&header.verticalResolution, 1, sizeof(DWORD), imageFile);
        fread(&header.numberOfColours, 1, sizeof(DWORD), imageFile);
        fread(&header.numberOfImportantColours, 1, sizeof(DWORD), imageFile);

        imageData = new BYTE[header.widht * header.height * 3];
        fseek(imageFile, header.offset, SEEK_SET);
        fread(imageData, 1, header.widht * header.height * 3, imageFile);

        imageLoaded = true;

        fclose(imageFile);

        cout << "Image loaded and header built. File path: " << filePath << endl;
        return true;
    }
    else
    {
        cout << "The file you are trying to read does not exist. File path: " << filePath << endl;
        return false;
    }
}

void ImageManipulator::saveImage(const char* filePath)
{

    if(imageLoaded)
    {

        FILE* imageFile = fopen(filePath ,"wb");

        if(imageFile)
        {
            fwrite(&header.identifier, sizeof(header.identifier), 1, imageFile);
            fwrite(&header.size, sizeof(header.size), 1, imageFile);
            fwrite(&header.reserved, sizeof(header.reserved), 1, imageFile);
            fwrite(&header.offset, sizeof(header.offset), 1, imageFile);
            fwrite(&header.headerSize, sizeof(header.headerSize), 1, imageFile);
            fwrite(&header.widht, sizeof(header.widht), 1, imageFile);
            fwrite(&header.height, sizeof(header.height), 1, imageFile);
            fwrite(&header.planes, sizeof(header.planes), 1, imageFile);
            fwrite(&header.bitsPerPixel, sizeof(header.bitsPerPixel), 1, imageFile);
            fwrite(&header.compression, sizeof(header.compression), 1, imageFile);
            fwrite(&header.imageSize, sizeof(header.imageSize), 1, imageFile);
            fwrite(&header.horizontalResolution, sizeof(header.horizontalResolution), 1, imageFile);
            fwrite(&header.verticalResolution, sizeof(header.verticalResolution), 1, imageFile);
            fwrite(&header.numberOfColours, sizeof(header.numberOfColours), 1, imageFile);
            fwrite(&header.numberOfImportantColours, sizeof(header.numberOfImportantColours), 1, imageFile);

            fseek(imageFile, header.offset, SEEK_SET);
            fwrite(imageData, 1, header.widht * header.height * 3, imageFile);

            fclose(imageFile);
            cout << "Image successfully created. File path: " << filePath << endl;

        }
        else
        {
            cout << "Impossible to create the file." << endl;
        }

    }
    else
    {
        cout << "You must first load the image that you want to save." << endl;
    }
}

void ImageManipulator::convertToGrayscale()
{
    if(imageLoaded)
    {

        BYTE intensity = 0;
        for(int y = 0; y < header.height; y++)
        {
            for(int x = 0; x < header.widht; x++)
            {
                Color color = getPixel(x, y);
                intensity = RGB2GRAY(color.r, color.g, color.b);

                color.r = intensity;
                color.g = intensity;
                color.b = intensity;

                setPixel(x, y, color);
            }
        }
        isGrayscale = true;
        cout << "Image successfully converted to grayscale," << endl;
    }
    else
    {
        cout << "You must first load the image that you want to convert to grayscale." << endl;
    }
}


void ImageManipulator::applyBrihtness(const int level)
{
    if(imageLoaded)
    {
        for(int y = 0; y < header.height; y++)
        {
            for(int x = 0; x < header.widht; x++)
            {
                Color color = getPixel(x, y);

                color.r = CLAMP(color.r + level);
                color.g = CLAMP(color.g + level);
                color.b = CLAMP(color.b + level);

                setPixel(x, y, color);
            }
        }
        isGrayscale = true;
        cout << "Image successfully changed the brightness," << endl;
    }
    else
    {
        cout << "You must first load the image that you want to change the brightness." << endl;
    }

}

Color ImageManipulator::getPixel(const int x, const int y)
{
    Color color;

    if(imageLoaded)
    {
        color.r = imageData[(y * header.widht * 3) + (x * 3) + 2];
        color.g = imageData[(y * header.widht * 3) + (x * 3) + 1];
        color.b = imageData[(y * header.widht * 3) + (x * 3)];

    }
    else
    {
        cout << "You must first load the image that you want to get the pixel." << endl;
    }

    return color;
}

void ImageManipulator::setPixel(const int x, const int y, const Color color)
{

    if(imageLoaded)
    {
        imageData[(y * header.widht * 3) + (x * 3)] = color.b;
        imageData[(y * header.widht * 3) + (x * 3) + 1] = color.g;
        imageData[(y * header.widht * 3) + (x * 3) + 2] = color.r;
    }
    else
    {
        cout << "You must first load the image that you want to set the pixel." << endl;
    }

}

void ImageManipulator::printHeader()
{

    if(imageLoaded)
    {
        cout << "Identifier: " << header.identifier << endl;
        cout << "File size: " << header.size << endl;
        cout << "Offset: " << header.offset << endl;
        cout << "Header size: " << header.headerSize << endl;
        cout << "Widht: " << header.widht << endl;
        cout << "Height: " << header.height << endl;
        cout << "Planes: " << header.planes << endl;
        cout << "Bits per pixel: " << header.bitsPerPixel << endl;
        cout << "Compression: " << header.compression << endl;
        cout << "Image size: " << header.imageSize << endl;
        cout << "Horizontal number of pixels: " << header.horizontalResolution << endl;
        cout << "Vertical number of pixels: " << header.verticalResolution << endl;
        cout << "Number of colours: " << header.numberOfColours << endl;
        cout << "Important number of colours: " << header.numberOfImportantColours << endl;

    }
    else
    {
        cout << "You must first load the image that you want to print the header" << endl;
    }

}

void ImageManipulator::printDataImage(const bool isTest, const int maxTestSize)
{
    int height;
    int widht;
    if(isTest)
    {
        if(header.height < maxTestSize)
        {
            height = header.height;
        }
        else
        {
            height = maxTestSize;
        }

        if(header.widht < maxTestSize)
        {
            widht = header.widht;
        }
        else
        {
            widht = maxTestSize;
        }
    }
    else
    {
        height = header.height;
        widht = header.widht;
    }
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < widht; x++)
        {
            Color color = getPixel(x, y);
            cout << "(" << (int)color.r << ", " << (int)color.g << ", " << (int)color.b << ")";
            cout << " ";

        }
        cout << endl;
    }
}

ImageManipulator ImageManipulator::applySobelsFilter(const int initialX, const int initialY, const int finalX, const int finalY)
{
    ImageManipulator imageSection;

    if(imageLoaded)
    {
        if(isGrayscale)
        {
            Sobel sobelFilter(*this);
            sobelFilter.applySobelFilter(initialX, initialY, finalX, finalY);
            imageSection = sobelFilter.edgeDetectedImage;

            cout << "Sobel filter was succesfully applied to the sector of the image: From (" <<  initialX << ", " << finalX << ")" << " To " <<  initialY << ", " << finalY << ")"<< endl;

        }
        else
        {
            cout << "You must first convert the image to grayscale. Calling convertToGrayscale() method..." << endl;
            convertToGrayscale();
            applySobelsFilter(initialX, initialY, finalX, finalY);

        }
    }
    else
    {
        cout << "You must first load the image that you want to apply the Sobel filter" << endl;
    }

    return imageSection;

}
