/*
SOBEL FILTER USING MPI
CENTRO UNIVERSITARIO DE CIENCIAS EXACTAS E INGIENERIAS
CONCURRENT AND DISTRIBUTED PROGRAMMING 2014A


Venegas Munoz Carlos
Navarro Palos Carlos Eduardo

 */
#include "ImageManipulator.h"
#include <string.h>
#include <mpi.h>
#include <sstream>  

using namespace imageManipulator;
using namespace std;

ImageManipulator::ImageManipulator() {
	imageData = NULL;
	greyImageData = NULL;
	imageLoaded = false;
	isGrayscale = false;
	_radius = 0;
	_theta = 45;
	_alpha = 25;
	_iterations = 30;

}

ImageManipulator::~ImageManipulator() {
}

ImageManipulator::ImageManipulator(const ImageManipulator& src) {

	imageLoaded = src.imageLoaded;
	isGrayscale = src.isGrayscale;
	header = src.header;
	imageData = new BYTE[src.header.width * src.header.height * 3];
	memcpy(imageData, src.imageData, src.header.width * src.header.height * 3);

	greyImageData = new BYTE[src.header.width * src.header.height * 3];
	memcpy(greyImageData, src.greyImageData, src.header.width * src.header.height * 3);

	_radius = src._radius;
	_theta = src._theta;
	_alpha = src._alpha;
	_iterations = src._iterations;
}

bool ImageManipulator::openImage(const char* filePath) {
	imageData = NULL;
	greyImageData = NULL;
	imageLoaded = false;

	imageFile = fopen(filePath, "rb");
	if (imageFile) {

		fread(&header.identifier, 1, sizeof (WORD), imageFile);
		fread(&header.size, 1, sizeof (DWORD), imageFile);
		fread(&header.reserved, 1, sizeof (DWORD), imageFile);
		fread(&header.offset, 1, sizeof (DWORD), imageFile);
		fread(&header.headerSize, 1, sizeof (DWORD), imageFile);
		fread(&header.width, 1, sizeof (DWORD), imageFile);
		fread(&header.height, 1, sizeof (DWORD), imageFile);
		fread(&header.planes, 1, sizeof (WORD), imageFile);
		fread(&header.bitsPerPixel, 1, sizeof (WORD), imageFile);

		if (header.bitsPerPixel != BITS_PER_PIXEL) {
			cout << "The image must have " << BITS_PER_PIXEL << " bits per pixel and this image has " << header.bitsPerPixel << " bits per pixel. Try with another image." << endl;
			return false;
		}
		fread(&header.compression, 1, sizeof (DWORD), imageFile);
		fread(&header.imageSize, 1, sizeof (DWORD), imageFile);
		fread(&header.horizontalResolution, 1, sizeof (DWORD), imageFile);
		fread(&header.verticalResolution, 1, sizeof (DWORD), imageFile);
		fread(&header.numberOfColours, 1, sizeof (DWORD), imageFile);
		fread(&header.numberOfImportantColours, 1, sizeof (DWORD), imageFile);

		imageData = new BYTE[header.width * header.height * 3];
		greyImageData = new BYTE[header.width * header.height * 3];

		fseek(imageFile, header.offset, SEEK_SET);
		fread(imageData, 1, header.width * header.height * 3, imageFile);

		fseek(imageFile, header.offset, SEEK_SET);
		fread(greyImageData, 1, header.width * header.height * 3, imageFile);

		imageLoaded = true;

		fclose(imageFile);

		cout << "Image loaded and header built. File path: " << filePath << endl;
		return true;
	} else {
		cout << "The file you are trying to read does not exist. File path: " << filePath << endl;
		return false;
	}
}

void ImageManipulator::saveImage(const char* filePath, bool grey) {

	if (imageLoaded) {

		FILE* imageFile = fopen(filePath, "wb");

		if (imageFile) {
			fwrite(&header.identifier, sizeof (header.identifier), 1, imageFile);
			fwrite(&header.size, sizeof (header.size), 1, imageFile);
			fwrite(&header.reserved, sizeof (header.reserved), 1, imageFile);
			fwrite(&header.offset, sizeof (header.offset), 1, imageFile);
			fwrite(&header.headerSize, sizeof (header.headerSize), 1, imageFile);
			fwrite(&header.width, sizeof (header.width), 1, imageFile);
			fwrite(&header.height, sizeof (header.height), 1, imageFile);
			fwrite(&header.planes, sizeof (header.planes), 1, imageFile);
			fwrite(&header.bitsPerPixel, sizeof (header.bitsPerPixel), 1, imageFile);
			fwrite(&header.compression, sizeof (header.compression), 1, imageFile);
			fwrite(&header.imageSize, sizeof (header.imageSize), 1, imageFile);
			fwrite(&header.horizontalResolution, sizeof (header.horizontalResolution), 1, imageFile);
			fwrite(&header.verticalResolution, sizeof (header.verticalResolution), 1, imageFile);
			fwrite(&header.numberOfColours, sizeof (header.numberOfColours), 1, imageFile);
			fwrite(&header.numberOfImportantColours, sizeof (header.numberOfImportantColours), 1, imageFile);

			fseek(imageFile, header.offset, SEEK_SET);
			if (grey == true) {
				fwrite(greyImageData, 1, header.width * header.height * 3, imageFile);
			} else {
				fwrite(imageData, 1, header.width * header.height * 3, imageFile);
			}

			fclose(imageFile);
			cout << "Image successfully created. File path: " << filePath << endl;

		} else {
			cout << "Impossible to create the file." << endl;
		}

	} else {
		cout << "You must first load the image that you want to save." << endl;
	}
}

void ImageManipulator::convertToGrayscale(const int initialX,
	const int initialY, const int finalX, const int finalY, int &rv, int gooch) {
	clock_t start, end;
	double duration;
	string intermFileName;
	stringstream ss;
	FILE *time_log = NULL;
	int rank;

	if (imageLoaded) {

		//convertToLab
		for (int y = initialY; y < finalY; y++) {
			for (int x = initialX; x < finalX; x++) {
				BYTE luminance = 0;

				Color color = getPixel(imageData, x, y);

				luminance = color.ToGrayscale();
				color.r = luminance;
				color.g = luminance;
				color.b = luminance;

				setPixel(greyImageData, x, y, color);
			}
		}

		if (gooch) {
			switch (gooch) {
				case 1:
					cout << "Converting to LAB..." << endl;
					convertTolab(initialX, initialY, finalX, finalY);
					break;
				case 2:
					cout << "Computing target diff..." << endl;
					computeTargetDifferences();
					break;
				case 3:
					cout << "Resolving Optimization Problem..." << endl;
					resolveOptimizationProblem();
					break;
				case 4:
					cout << "Converting to RGB..." << endl;
					convertToRgb(initialX, initialY, finalX, finalY);
					break;
				default:
					cout << "Applying all GOOCH..." << endl;
					convertTolab(initialX, initialY, finalX, finalY);
					computeTargetDifferences();
					resolveOptimizationProblem();
					convertToRgb(initialX, initialY, finalX, finalY);
					

			}

		}

		isGrayscale = true;
		rv = 0;
	} else {
		cout << "You must first load the image that you want to convert to greyscale." << endl;
		rv = -1;
	}
}

void ImageManipulator::computeTargetDifferences() {
	int N = getWidth() * getHeight();
	int rank;
	_deltas = new double[N];
	/*free me!!!*/

	//initialize to zeroed values
	for (int i = 0; i < N; ++i)
		_deltas[i] = 0.0;
	
	if (_radius) {
		for (int x = getWidth() - 1; x >= 0; x--) {
			for (int y = getHeight() - 1; y >= 0; y--) {
				int i = x + y * getWidth(); //position of first relative pixel
				int x_start = x - _radius;
				int x_end = x + _radius;
				int y_start = y - _radius;
				int y_end = y + _radius;
				for (int xx = x_start; xx <= x_end; xx++) {
					if (xx >= getWidth() || xx < 0)
						continue;
					for (int yy = y_start; yy <= y_end; yy++) {
						if (yy >= getHeight() || yy < 0)
							continue;
						int j = xx + yy * getWidth();
						double little_delta = computeDelta(i, j);
						_deltas[i] += little_delta;
						_deltas[j] -= little_delta;
					}
				}
			}
		}
	} else {
		
		for (int x = N - 1; x >= 0; x--) {
			for (int y = x + 1; y < N; ++y) {
				double little_delta = computeDelta(x, y);
				_deltas[x] += little_delta;
				_deltas[y] -= little_delta;
			}
		}
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	cout << "Successfully computed target diff " << rank << endl;
}

void ImageManipulator::convertTolab(const int initialX,
	const int initialY, const int finalX, const int finalY) {

	_data = new CIE_Lab[getWidth() * getHeight()];
	int itr = 0;
	int rank;
	
	//convertToLab
	for (int y = initialY; y < finalY; y++) {
		for (int x = initialX; x < finalX; x++) {
			Color color = getPixel(imageData, x, y);

			CIE_Lab temp(color);
			_data[itr].a = temp.a;
			_data[itr].b = temp.b;
			_data[itr].L = temp.L;
			itr++;
		}
	}
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	cout << "Successfully converted to lab for " << rank << endl;

}

void ImageManipulator::writeLabData() {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	string fileName;
	stringstream ss;
	ss << "lab";
	ss << rank;
	ss << ".data";
	fileName = ss.str();
	FILE *labFile = fopen(fileName.c_str(), "wb");
	for (int i = 0; i < getWidth() * getHeight(); ++i) {
		fprintf(labFile, "%f %f %f", _data[i].a, _data[i].b, _data[i].L);
	}
	delete _data;
}

void ImageManipulator::writeDeltasData() {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	string fileName;
	stringstream ss;
	ss << "deltas";
	ss << rank;
	ss << ".data";
	fileName = ss.str();
	FILE *deltasFile = fopen(fileName.c_str(), "wb");
	for (int i = 0; i < getWidth() * getHeight(); ++i) {
		fprintf(deltasFile, "%lf ", _deltas[i]);
	}
	delete _deltas;
}

void ImageManipulator::writeOptimizedData() {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	string fileName;
	stringstream ss;
	ss << "optimized";
	ss << rank;
	ss << ".data";
	fileName = ss.str();
	FILE *optimzedFile = fopen(fileName.c_str(), "wb");
	for (int i = 0; i < getWidth() * getHeight(); ++i) {
		fprintf(optimzedFile, "%lf ", _dataOutput[i]);
	}
	delete _dataOutput;
}

void ImageManipulator::readDeltasData() {
	int rank;
	_deltas = new double[getWidth() * getHeight()];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	string fileName;
	stringstream ss;
	ss << "deltas";
	ss << rank;
	ss << ".data";
	fileName = ss.str();
	FILE *deltasFile = fopen(fileName.c_str(), "rb");
	for (int i = 0; i < getWidth() * getHeight(); ++i) {
		fscanf(deltasFile, "%lf ", &_deltas[i]);
	}
}

void ImageManipulator::readLabData() {
	int rank;
	_data = new CIE_Lab[getWidth() * getHeight()];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	string fileName;
	stringstream ss;
	ss << "lab";
	ss << rank;
	ss << ".data";
	fileName = ss.str();
	FILE *labFile = fopen(fileName.c_str(), "rb");
	for (int i = 0; i < getWidth() * getHeight(); ++i) {
		fscanf(labFile, "%lf %lf %lf", &_data[i].a, &_data[i].b, &_data[i].L);
	}
}

void ImageManipulator::readOptimizedData() {
	int rank;
	_dataOutput = new double[getWidth() * getHeight()];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	string fileName;
	stringstream ss;
	ss << "optimized";
	ss << rank;
	ss << ".data";
	fileName = ss.str();
	FILE *optimizedFile = fopen(fileName.c_str(), "rb");
	for (int i = 0; i < getWidth() * getHeight(); ++i) {
		fscanf(optimizedFile, "%lf ", &_dataOutput[i]);
	}
}

void ImageManipulator::convertToRgb(const int initialX,
	const int initialY, const int finalX, const int finalY) {
	unsigned int N = getWidth() * getHeight();

	_greyOutput = new Color[N];
	for (unsigned int i = 0; i < N; ++i) {
		_greyOutput[i] = CIE_Lab(_dataOutput[i], 0, 0).toRGB();

	}
	SetGreyOutput(initialX, initialY, finalX, finalY);

}

void ImageManipulator::SetGreyOutput(const int initialX,
	const int initialY, const int finalX, const int finalY) {

	int itr = 0;

	for (int y = initialY; y < finalY; y++) {
		for (int x = initialX; x < finalX; x++) {
			Color color = getPixel(imageData, x, y);
			Color greyPixel = _greyOutput[itr];
			color.r = greyPixel.r;
			color.g = greyPixel.g;
			color.b = greyPixel.b;
			setPixel(greyImageData, x, y, color);
			itr++;
		}
	}

}

void ImageManipulator::resolveOptimizationProblem() {
	int N = getWidth() * getHeight();

	//allocate the CIE_Lab output and free it
	_dataOutput = new double[N];

	/*show intermediate progress based on iterations*/
	/*initializing the gray image to the luminance values*/
	for (int i = 0; i < N; ++i)
		_dataOutput[i] = _data[i].L;

	if (_radius) {
		for (int iter = this->_iterations; iter != 0; iter--) {
			for (int x = getWidth() - 1; x >= 0; x--) {
				for (int y = getHeight() - 1; y >= 0; y--) {
					int i = x + y * getWidth(); //position of the relative pixel
					int x_start = x - _radius;
					int x_end = x + _radius;
					int y_start = y - _radius;
					int y_end = y + _radius;
					double sum = 0;
					int count = 0;
					for (int xx = x_start; xx <= x_end; ++xx) {
						if (xx >= getWidth() || xx < 0)
							continue;
						for (int yy = y_start; yy <= y_end; ++yy) {
							if (yy >= getHeight() || yy < 0)
								continue;
							int j = xx + yy * getWidth();
							sum += _dataOutput[j];
							count++;
						}
					}
					_dataOutput[i] = (_deltas[i] + sum) / (double) count;
				}
			}
		}
	} else {
		for (int x = 1; x < N; ++x) {
			_dataOutput[x] = (_deltas[x] - _deltas[x - 1] + N * _dataOutput[x - 1]) / (double) N;
		}
	}
}

double ImageManipulator::crunch(double x) {
	return _alpha == 0 ? 0 : (_alpha * tanh(x / _alpha));
}

double ImageManipulator::computeDelta(int i, int j) {
	double dL = _data[i].L - _data[j].L;
	double cosTheta = cos(_theta);
	double sinTheta = sin(_theta);
	double a = _data[i].a - _data[j].a;
	double b = _data[i].b - _data[j].b;
	double dC = sqrt(a * a + b * b);

	double crunchX = crunch(dC);

	if (fabs(dL) > crunchX) {
		return dL;
	}

	double vTheta = cosTheta * a + sinTheta*b;
	if ((vTheta * crunchX) > 0)
		return crunchX;
	return -crunchX;
}

void ImageManipulator::applyBrightness(const int level) {
	if (imageLoaded) {
		for (int y = 0; y < header.height; y++) {
			for (int x = 0; x < header.width; x++) {
				Color color = getPixel(imageData, x, y);

				color.r = CLAMP(color.r + level);
				color.g = CLAMP(color.g + level);
				color.b = CLAMP(color.b + level);

				setPixel(greyImageData, x, y, color);
			}
		}
		isGrayscale = true;
		cout << "Image successfully changed the brightness," << endl;
	} else {
		cout << "You must first load the image that you want to change the brightness." << endl;
	}

}

Color ImageManipulator::getPixel(BYTE* imageData, const int x, const int y) {
	Color color;

	if (imageLoaded) {
		color.r = imageData[(y * header.width * 3) + (x * 3) + 2];
		color.g = imageData[(y * header.width * 3) + (x * 3) + 1];
		color.b = imageData[(y * header.width * 3) + (x * 3)];

	} else {
		cout << "You must first load the image that you want to get the pixel." << endl;
	}

	return color;
}

void ImageManipulator::setPixel(BYTE* imageData, const int x, const int y,
	const Color color) {

	if (imageLoaded) {
		imageData[(y * header.width * 3) + (x * 3)] = color.b;
		imageData[(y * header.width * 3) + (x * 3) + 1] = color.g;
		imageData[(y * header.width * 3) + (x * 3) + 2] = color.r;
	} else {
		cout << "You must first load the image that you want to set the pixel." << endl;
	}

}

void ImageManipulator::printHeader() {

	if (imageLoaded) {
		cout << "Identifier: " << header.identifier << endl;
		cout << "File size: " << header.size << endl;
		cout << "Offset: " << header.offset << endl;
		cout << "Header size: " << header.headerSize << endl;
		cout << "Widht: " << header.width << endl;
		cout << "Height: " << header.height << endl;
		cout << "Planes: " << header.planes << endl;
		cout << "Bits per pixel: " << header.bitsPerPixel << endl;
		cout << "Compression: " << header.compression << endl;
		cout << "Image size: " << header.imageSize << endl;
		cout << "Horizontal number of pixels: " << header.horizontalResolution << endl;
		cout << "Vertical number of pixels: " << header.verticalResolution << endl;
		cout << "Number of colours: " << header.numberOfColours << endl;
		cout << "Important number of colours: " << header.numberOfImportantColours << endl;

	} else {
		cout << "You must first load the image that you want to print the header" << endl;
	}

}

DWORD ImageManipulator::getWidth() {
	return header.width;
}

DWORD ImageManipulator::getHeight() {
	return header.height;
}

void ImageManipulator::setHeight(DWORD height) {
	header.height = height;
}

void ImageManipulator::printDataImage(const bool isTest, const int maxTestSize) {
	int height;
	int widht;
	if (isTest) {
		if (header.height < maxTestSize) {
			height = header.height;
		} else {
			height = maxTestSize;
		}

		if (header.width < maxTestSize) {
			widht = header.width;
		} else {
			widht = maxTestSize;
		}
	} else {
		height = header.height;
		widht = header.width;
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < widht; x++) {
			Color color = getPixel(imageData, x, y);
			cout << "(" << (int) color.r << ", " << (int) color.g << ", " << (int) color.b << ")";
			cout << " ";

		}
		cout << endl;
	}
}

void ImageManipulator::applySobelsFilter(const int initialX,
	const int initialY, const int finalX, const int finalY, int &rv) {
	if (imageLoaded) {
		if (isGrayscale) {

			BYTE gradientR = 0;
			BYTE gradientG = 0;
			BYTE gradientB = 0;
			BYTE maxValuePixelR = 0;
			BYTE maxValuePixelG = 0;
			BYTE maxValuePixelB = 0;

			for (int y = initialY; y < finalY; y++) {
				for (int x = initialX; x < finalX; x++) {
					Color pixel = getPixel(imageData, x, y);

					/**
					These gx and gy should be retrieved using the convolution matrix from definitions.h but this is a fastest way to calculate the gradient.
					 **/

					float bx = (((2 * getPixel(imageData, x + 2, y + 2).b) +
						getPixel(imageData, x + 2, y).b +
						getPixel(imageData, x + 2, y + 2).b) -
						(2 * getPixel(imageData, x, y + 1).b +
						getPixel(imageData, x, y).b +
						getPixel(imageData, x, y + 2).b));

					float gx = (((2 * getPixel(imageData, x + 2, y + 2).g) +
						getPixel(imageData, x + 2, y).g +
						getPixel(imageData, x + 2, y + 2).g) -
						(2 * getPixel(imageData, x, y + 1).g +
						getPixel(imageData, x, y).g +
						getPixel(imageData, x, y + 2).g));

					float rx = (((2 * getPixel(imageData, x + 2, y + 2).r) +
						getPixel(imageData, x + 2, y).r +
						getPixel(imageData, x + 2, y + 2).r) -
						(2 * getPixel(imageData, x, y + 1).r +
						getPixel(imageData, x, y).r +
						getPixel(imageData, x, y + 2).r));

					float by = (((2 * getPixel(imageData, x + 1, y + 2).b) +
						getPixel(imageData, x, y + 2).b +
						getPixel(imageData, x + 2, y + 2).b) -
						(2 * getPixel(imageData, x + 1, y).b +
						getPixel(imageData, x, y).b +
						getPixel(imageData, x + 2, y).b));

					float gy = (((2 * getPixel(imageData, x + 1, y + 2).g) +
						getPixel(imageData, x, y + 2).g +
						getPixel(imageData, x + 2, y + 2).g) -
						(2 * getPixel(imageData, x + 1, y).g +
						getPixel(imageData, x, y).g +
						getPixel(imageData, x + 2, y).g));

					float ry = (((2 * getPixel(imageData, x + 1, y + 2).r) +
						getPixel(imageData, x, y + 2).r +
						getPixel(imageData, x + 2, y + 2).r) -
						(2 * getPixel(imageData, x + 1, y).r +
						getPixel(imageData, x, y).r +
						getPixel(imageData, x + 2, y).r));


					if (gx < 0) {
						gx = -gx;
					}
					if (gy < 0) {
						gy = -gy;
					}

					gradientR = (BYTE) round(sqrt(pow(rx, 2) + pow(ry, 2)));
					gradientG = (BYTE) round(sqrt(pow(gx, 2) + pow(gy, 2)));
					gradientB = (BYTE) round(sqrt(pow(bx, 2) + pow(by, 2)));

					pixel.r = gradientR;
					pixel.g = gradientG;
					pixel.b = gradientB;
					if (gradientR > maxValuePixelR) {
						maxValuePixelR = gradientR;
					}
					if (gradientG > maxValuePixelG) {
						maxValuePixelG = gradientG;
					}
					if (gradientB > maxValuePixelB) {
						maxValuePixelB = gradientB;
					}
					setPixel(greyImageData, x, y, pixel);
				}
			}
			//scale the values 
			for (int y = initialY; y < finalY; y++) {
				for (int x = initialX; x < finalX; x++) {
					Color pixel = getPixel(greyImageData, x, y);
					BYTE oldValueR = (pixel.r * 255) / maxValuePixelR;
					BYTE oldValueG = (pixel.g * 255) / maxValuePixelG;
					BYTE oldValueB = (pixel.b * 255) / maxValuePixelB;

					pixel.r = oldValueR;
					pixel.g = oldValueG;
					pixel.b = oldValueB;
					setPixel(greyImageData, x, y, pixel);
				}
			}

			cout << "Sobel filter was succesfully applied to the sector of the "
				"image: From (" << initialX << ", " << finalX << ")" << " To "
				<< initialY << ", " << finalY << ")" << endl;
			rv = 0;

		} else {
			cout << "You must first convert the image to grayscale. "
				"Calling convertToGrayscale() method..." << endl;
			rv = -1;

		}
	} else {
		cout << "You must first load the image that you want to apply the Sobel filter" << endl;
		rv = -1;
	}

}
