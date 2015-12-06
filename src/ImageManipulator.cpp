/*
 * MPI Gooch Algorithm for Converting to Greyscale
 * Diana Popa
 * Polytechnica University of Bucharest
 */

#include "ImageManipulator.h"
#include <string.h>
#include <mpi.h>
#include <sstream>
#include "bitmap_image.hpp"

using namespace imageManipulator;
using namespace std;

ImageManipulator::ImageManipulator() {

	imageData = NULL;
	greyImageData = NULL;

	isGrayscale = false;
	_radius = 0;
	_theta = 45;
	_alpha = 15;
	_iterations = 10;
	_cosTheta = cos(_theta);
	_sinTheta = sin(_theta);

}

ImageManipulator::~ImageManipulator() {
	if (0 != imageData) {
		delete[] imageData;
	}
	if (0 != greyImageData) {
		delete[] greyImageData;
	}
}

ImageManipulator::ImageManipulator(const ImageManipulator& src) {

	isGrayscale = src.isGrayscale;
	_width = src._width;
	_height = src._height;

	imageData = new BYTE[_width * _height * 3];
	memcpy(imageData, src.imageData, _width * _height * 3);

	greyImageData = new BYTE[_width * _height * 3];
	memcpy(greyImageData, src.greyImageData, _width * _height * 3);

	_radius = src._radius;
	_theta = src._theta;
	_alpha = src._alpha;
	_iterations = src._iterations;
	_cosTheta = src._cosTheta;
	_sinTheta = src._sinTheta;
}

bool ImageManipulator::openImage(bitmap_image image) {

	imageData = new BYTE[image.width() * image.height() * 3];
	greyImageData = new BYTE[image.width() * image.height() * 3];

	_width = image.width();
	_height = image.height();

	memcpy(imageData, image.data(), image.width() * image.height() * 3);
}

void ImageManipulator::saveImage(string file_name, bitmap_image image) {

	image.copy_from(greyImageData);
	image.save_image(file_name);
}

void ImageManipulator::convertToGrayscale(const int initialX,
	const int initialY, const int finalX, const int finalY, int gooch) {

	//convertTolab(initialX,  initialY, finalX, finalY);
	//convertToRgb(initialX,  initialY, finalX, finalY, gooch);

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
				convertToRgb(initialX, initialY, finalX, finalY, gooch);
				break;
			default:
				cout << "Applying all GOOCH..." << endl;
				convertTolab(initialX, initialY, finalX, finalY);
				computeTargetDifferences();
				resolveOptimizationProblem();
				convertToRgb(initialX, initialY, finalX, finalY, gooch);
		}

	}

	isGrayscale = true;
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
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	string fileName;
	stringstream ss;
	ss << "lab";
	ss << rank;
	ss << "_";
	ss << size;
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
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	string fileName;
	stringstream ss;
	ss << "deltas";
	ss << rank;
	ss << "_";
	ss << size;

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
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	string fileName;
	stringstream ss;
	ss << "optimized";
	ss << rank;
	ss << "_";
	ss << size;

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
	int size;
	_deltas = new double[getWidth() * getHeight()];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	string fileName;
	stringstream ss;
	ss << "deltas";
	ss << rank;
	ss << "_";
	ss << size;

	ss << ".data";
	fileName = ss.str();
	FILE *deltasFile = fopen(fileName.c_str(), "rb");
	for (int i = 0; i < getWidth() * getHeight(); ++i) {
		fscanf(deltasFile, "%lf ", &_deltas[i]);
	}
}

void ImageManipulator::readLabData() {
	int rank;
	int size;
	_data = new CIE_Lab[getWidth() * getHeight()];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	string fileName;
	stringstream ss;
	ss << "lab";
	ss << rank;
	ss << "_";
	ss << size;

	ss << ".data";
	fileName = ss.str();
	FILE *labFile = fopen(fileName.c_str(), "rb");
	for (int i = 0; i < getWidth() * getHeight(); ++i) {
		fscanf(labFile, "%lf %lf %lf", &_data[i].a, &_data[i].b, &_data[i].L);
	}
}

void ImageManipulator::readOptimizedData() {
	int rank;
	int size;
	_dataOutput = new double[getWidth() * getHeight()];
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	string fileName;
	stringstream ss;
	ss << "optimized";
	ss << rank;
	ss << "_";
	ss << size;

	ss << ".data";
	fileName = ss.str();
	FILE *optimizedFile = fopen(fileName.c_str(), "rb");
	for (int i = 0; i < getWidth() * getHeight(); ++i) {
		fscanf(optimizedFile, "%lf ", &_dataOutput[i]);
	}
}

void ImageManipulator::convertToRgb(const int initialX,
	const int initialY, const int finalX, const int finalY, int gooch) {
	unsigned int N = getWidth() * getHeight();

	_greyOutput = new Color[N];
	for (unsigned int i = 0; i < N; ++i) {
		if (gooch)
			_greyOutput[i] = CIE_Lab(_dataOutput[i], 0, 0).toRGB();
		else
			_greyOutput[i] = CIE_Lab(_data[i].L, 0, 0).toRGB();
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
			setPixel(greyImageData, x, y, greyPixel);
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

	_radius = getWidth();


	if (_radius) {

		for (int iter = 0; iter < _iterations; iter++) {

			for (int x = 0; x < getWidth(); x++) {
				for (int y = 0; y < getHeight(); y++) {

					int i = x + y * getWidth(); //position of the relative pixel

					int x_start = x - _radius;
					int x_end = x + _radius;

					int y_start = y - _radius;
					int y_end = y + _radius;

					double sum = 0;
					int count = 0;

					for (int xx = x_start; xx <= x_end; xx++) {

						if (xx >= getWidth() || xx < 0)
							continue;
						for (int yy = y_start; yy <= y_end; yy++) {

							if (yy >= getHeight() || yy < 0)
								continue;

							int j = xx + yy * getWidth();
							sum += _dataOutput[j];
							count++;
						}
					}
					_dataOutput[i] = (_deltas[i] + sum) / (double) count;

					if (_dataOutput[i] > 255) _dataOutput[i] = 255;
					if (_dataOutput[i] < 0) _dataOutput[i] = 0;
				}
			}
		}
	} else {
		for (int iter = 0; iter < 30; iter++) {
			for (int x = 1; x < N; ++x) {
				double t1 = _deltas[x] / (double) N;
				double t2 = _deltas[x - 1] / (double) N;
				//_dataOutput[x] = (_deltas[x] - _deltas[x - 1] + N * _dataOutput[x - 1]) / (double) N;
				_dataOutput[x] = t1 - t2 + _dataOutput[x - 1];
				if (_dataOutput[x] > 255) _dataOutput[x] = 255;
				if (_dataOutput[x] < 0) _dataOutput[x] = 0;
			}
		}
	}
}

double ImageManipulator::crunch(double x) {
	return _alpha == 0 ? 0 : (_alpha * tanh(x / _alpha));
}

double ImageManipulator::computeDelta(int i, int j) {

	double dL = _data[i].L - _data[j].L;

	double a = _data[i].a - _data[j].a;
	double b = _data[i].b - _data[j].b;
	double dC = sqrt(a * a + b * b);

	double crunchX = crunch(dC);

	if (fabs(dL) > crunchX) {
		return dL;
	}

	double vTheta = _cosTheta * a + _sinTheta*b;
	if ((vTheta * crunchX) > 0)
		return crunchX;
	return -crunchX;
}

void ImageManipulator::applyBrightness(const int level) {
	for (int y = 0; y < _height; y++) {
		for (int x = 0; x < _width; x++) {
			Color color = getPixel(imageData, x, y);

			color.r = CLAMP(color.r + level);
			color.g = CLAMP(color.g + level);
			color.b = CLAMP(color.b + level);

			setPixel(greyImageData, x, y, color);
		}
	}
	isGrayscale = true;
	cout << "Image successfully changed the brightness!" << endl;
}

Color ImageManipulator::getPixel(BYTE* data, const int x, const int y) {
	Color color;

	color.r = data[(y * _width * 3) + (x * 3) + 2];
	color.g = data[(y * _width * 3) + (x * 3) + 1];
	color.b = data[(y * _width * 3) + (x * 3)];


	return color;
}

void ImageManipulator::setPixel(BYTE* data, const int x, const int y,
	const Color color) {

	data[(y * _width * 3) + (x * 3)] = color.b;
	data[(y * _width * 3) + (x * 3) + 1] = color.g;
	data[(y * _width * 3) + (x * 3) + 2] = color.r;
}

DWORD ImageManipulator::getWidth() {
	return _width;
}

DWORD ImageManipulator::getHeight() {
	return _height;
}

void ImageManipulator::setHeight(DWORD height) {
	_height = height;
}

void ImageManipulator::printDataImage() {
	int height;
	int width;
	height = _height;
	width = _width;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			//Color color = getPixel(imageData, x, y);
			Color color = getPixel(greyImageData, x, y);
			cout << "(" << (int) color.r << ", " << (int) color.g << ", " << (int) color.b << ")";
			cout << " ";

		}
		cout << endl;
	}
}