/*
 * MPI Gooch Algorithm for Converting to Greyscale
 * Diana Popa
 * Polytechnica University of Bucharest
 */

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "ImageManipulator.h"
#include "Definitions.h"
#include <sstream>

#define NAME_LENGTH 11

using namespace imageManipulator;
using namespace std;

int rv;

int main(int argc, char *argv[]) {
	if (argc != 3) {
		cout << "Usage: mpirun -np [task#] ./exec input_file step" << endl;
		return -1;
	}

	ifstream hostsFile("hosts");
	string line;
	int numberHosts = 0;
	int worldSize;
	int rank;
	char processorName[MPI_MAX_PROCESSOR_NAME];
	int nameLength;

	if (hostsFile) {
		while (getline(hostsFile, line)) {
			numberHosts++;
		}
	} else {
		cout << "Could not read hosts file." << endl;
		return -1;
	}

	rv = MPI_Init(&argc, &argv);

	if (rv != MPI_SUCCESS) {
		cout << "Error Initializing MPI Environment";
		MPI_Abort(MPI_COMM_WORLD, rv);
		return -1;
	}
	// Get the number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
	// Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// Get the name of the processor
	MPI_Get_processor_name(processorName, &nameLength);

	string input_file(argv[1]);
	string step_str(argv[2]);
	int step;
	istringstream buffer(step_str);
	buffer >> step;
	
	clock_t start, end;
	double duration;
	string timeFileName;
	stringstream ss;
	FILE *time_log = NULL;

	ImageManipulator imageManipulator;
	int sectionHeight;
	string grayscaleFileName;

	switch (step) {
		case 0:
		{
			imageManipulator.openImage(input_file.c_str());


			printf("Image width: %d \nImage Height: %d\n", imageManipulator.getWidth(),
				imageManipulator.getHeight());

			sectionHeight = imageManipulator.getHeight() / worldSize;

			ImageManipulator littleIm(imageManipulator);
			littleIm.setHeight((DWORD) sectionHeight);
			littleIm.convertToGrayscale(0,
				rank * sectionHeight, littleIm.getWidth(),
				((rank + 1) * sectionHeight), rv, step);

			if (rv) {
				cout << "Conversion to basic grayscale failed for " << rank << "th processor" << endl;
				MPI_Finalize();
				return -1;
			}


			MPI_Allgather(littleIm.greyImageData + sectionHeight * rank * littleIm.getWidth()*3,
				sectionHeight * littleIm.getWidth()*3, MPI_CHAR, imageManipulator.greyImageData,
				sectionHeight * littleIm.getWidth()*3, MPI_CHAR, MPI_COMM_WORLD);

			if (rank == 0) {
				grayscaleFileName = input_file;
				grayscaleFileName.insert(input_file.size() - 4, "_basic_grayscale");
				imageManipulator.saveImage(grayscaleFileName.c_str(), true);
			}
			break;
		}
		case 1:
		{
			//1 means that the algorithm will only apply the first step:convertToLab
			imageManipulator.openImage(input_file.c_str());

			printf("Image width: %d \nImage Height: %d\n", imageManipulator.getWidth(),
				imageManipulator.getHeight());

			sectionHeight = imageManipulator.getHeight() / worldSize;
			ImageManipulator littleIm(imageManipulator);
			littleIm.setHeight((DWORD) sectionHeight);

			MPI_Barrier(MPI_COMM_WORLD);
			start = clock();
			littleIm.convertToGrayscale(0,
				rank * sectionHeight, littleIm.getWidth(),
				((rank + 1) * sectionHeight), rv, step);
			MPI_Barrier(MPI_COMM_WORLD);
			end = clock();
			if (rank == 0) {
				duration = (double) (end - start) / CLOCKS_PER_SEC;
				ss << "logs/convertToLab";
				ss << worldSize;
				timeFileName = ss.str();
				time_log = fopen(timeFileName.c_str(), "wb");
				fprintf(time_log, "Total time taken by CPU: %f\n", duration);
				fclose(time_log);
			}
			if (rv) {
				cout << "Conversion to CIE lab failed for " << rank << "th processor" << endl;
				MPI_Finalize();
				return -1;
			}
			littleIm.writeLabData();
			break;
		}
		case 2:
		{
			//1 means that the algorithm will only apply the second step
			imageManipulator.openImage(input_file.c_str());

			printf("Image width: %d \nImage Height: %d\n", imageManipulator.getWidth(),
				imageManipulator.getHeight());

			sectionHeight = imageManipulator.getHeight() / worldSize;
			ImageManipulator littleIm(imageManipulator);
			littleIm.setHeight((DWORD) sectionHeight);
			littleIm.readLabData();

			MPI_Barrier(MPI_COMM_WORLD);
			start = clock();
			littleIm.convertToGrayscale(0,
				rank * sectionHeight, littleIm.getWidth(),
				((rank + 1) * sectionHeight), rv, step);
			MPI_Barrier(MPI_COMM_WORLD);
			end = clock();
			if (rank == 0) {
				duration = (double) (end - start) / CLOCKS_PER_SEC;
				ss << "logs/computeTargetDiff";
				ss << worldSize;
				timeFileName = ss.str();
				time_log = fopen(timeFileName.c_str(), "wb");
				fprintf(time_log, "Total time taken by CPU: %f\n", duration);
				fclose(time_log);
			}
			littleIm.writeDeltasData();
			break;
		}
		case 3:
		{
			//resolve optimization
			//1 means that the algorithm will only apply the second step
			imageManipulator.openImage(input_file.c_str());

			printf("Image width: %d \nImage Height: %d\n", imageManipulator.getWidth(),
				imageManipulator.getHeight());

			sectionHeight = imageManipulator.getHeight() / worldSize;
			ImageManipulator littleIm(imageManipulator);
			littleIm.setHeight((DWORD) sectionHeight);
			
			littleIm.readLabData();
			littleIm.readDeltasData();

			MPI_Barrier(MPI_COMM_WORLD);
			start = clock();
			littleIm.convertToGrayscale(0,
				rank * sectionHeight, littleIm.getWidth(),
				((rank + 1) * sectionHeight), rv, step);
			MPI_Barrier(MPI_COMM_WORLD);
			end = clock();
			if (rank == 0) {
				duration = (double) (end - start) / CLOCKS_PER_SEC;
				ss << "logs/computeOptimization";
				ss << worldSize;
				timeFileName = ss.str();
				time_log = fopen(timeFileName.c_str(), "wb");
				fprintf(time_log, "Total time taken by CPU: %f\n", duration);
				fclose(time_log);
			}
			littleIm.writeOptimizedData();
			break;
		}
		case 4:
		{
			//covnert back to RGB
			imageManipulator.openImage(input_file.c_str());

			printf("Image width: %d \nImage Height: %d\n", imageManipulator.getWidth(),
				imageManipulator.getHeight());

			sectionHeight = imageManipulator.getHeight() / worldSize;
			ImageManipulator littleIm(imageManipulator);
			littleIm.setHeight((DWORD) sectionHeight);
			littleIm.readOptimizedData();
			
			MPI_Barrier(MPI_COMM_WORLD);
			start = clock();
			littleIm.convertToGrayscale(0,
				rank * sectionHeight, littleIm.getWidth(),
				((rank + 1) * sectionHeight), rv, step);
			MPI_Barrier(MPI_COMM_WORLD);
			end = clock();
			
			
			MPI_Gather(littleIm.greyImageData + sectionHeight * rank * littleIm.getWidth()*3,
				sectionHeight * littleIm.getWidth()*3, MPI_CHAR, imageManipulator.greyImageData,
				sectionHeight * littleIm.getWidth()*3, MPI_CHAR, 0, MPI_COMM_WORLD);

			if (rank == 0) {
				duration = (double) (end - start) / CLOCKS_PER_SEC;
				ss << "logs/convertToRgb";
				ss << worldSize;
				timeFileName = ss.str();
				time_log = fopen(timeFileName.c_str(), "wb");
				fprintf(time_log, "Total time taken by CPU: %f\n", duration);
				fclose(time_log);

				grayscaleFileName = input_file;
				grayscaleFileName.insert(input_file.size() - 4, "_gooch_grayscale");
				imageManipulator.saveImage(grayscaleFileName.c_str(), true);
			}
			break;

		}
		default:
			imageManipulator.openImage(input_file.c_str());

			printf("Image width: %d \nImage Height: %d\n", imageManipulator.getWidth(),
				imageManipulator.getHeight());

			sectionHeight = imageManipulator.getHeight() / worldSize;
			ImageManipulator littleIm(imageManipulator);
			littleIm.setHeight((DWORD) sectionHeight);
			
			MPI_Barrier(MPI_COMM_WORLD);
			start = clock();
			littleIm.convertToGrayscale(0,
				rank * sectionHeight, littleIm.getWidth(),
				((rank + 1) * sectionHeight), rv, step);
			MPI_Barrier(MPI_COMM_WORLD);
			end = clock();
			
			
			MPI_Gather(littleIm.greyImageData + sectionHeight * rank * littleIm.getWidth()*3,
				sectionHeight * littleIm.getWidth()*3, MPI_CHAR, imageManipulator.greyImageData,
				sectionHeight * littleIm.getWidth()*3, MPI_CHAR, 0, MPI_COMM_WORLD);

			if (rank == 0) {
				duration = (double) (end - start) / CLOCKS_PER_SEC;
				ss << "logs/allGOOCH";
				ss << worldSize;
				timeFileName = ss.str();
				time_log = fopen(timeFileName.c_str(), "wb");
				fprintf(time_log, "Total time taken by CPU: %f\n", duration);
				fclose(time_log);

				grayscaleFileName = input_file;
				grayscaleFileName.insert(input_file.size() - 4, "_all_gooch_grayscale");
				imageManipulator.saveImage(grayscaleFileName.c_str(), true);
			}
			break;

	}

	// Finalize the MPI environment.
	MPI_Finalize();

	return 0;
}
