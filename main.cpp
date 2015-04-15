/*
 * Student: Mardaloescu Serban
 * Grupa: 334CA
 */

#include <iostream>
#include <mpi.h>
#include <fstream>
#include "complex.h"
#include "info.h"

#define NO_COLORS 256
#define info_size 11

using namespace std;

//Functions prototypes
int compute_Mandelbrot(int i, int j, Info *info);
int compute_Julia(int i, int j, Info *info);

int main(int argc, char** argv) {
	int rank, size;

	//Initialize the MPI World
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	MPI_Request req;

	double step, julia_param_1, julia_param_2;
	int max_steps;
	double coords[4];
	int width, height;
	int set_type;
	int lines_to_compute, lines_left, row;
	Info info;
	int *help_array;

	if(rank == 0) { //Master
		int **color_matrix;
		ifstream in(argv[1]);
		ofstream out(argv[2]);
		//The master process reads from the input file
		in >> set_type;
		in >> coords[0]; in >> coords[1]; in >> coords[2]; in >> coords[3];
		in >> step;
		in >> max_steps;
		if(set_type == 1) {
			in >> julia_param_1;
			in >> julia_param_2;
		}

		//Calculate the width and height of the final matrix
		width = floor((coords[1] - coords[0]) / step);
		height = floor((coords[3] - coords[2]) / step);

		//Populate the informational class with relevant information
		//for the workers
		info.set_type = set_type;
		info.x_min = coords[0];
		info.y_min = coords[2];
		info.step = step;
		info.max_steps = max_steps;
		info.width = width;
		info.height = height;
		if(set_type == 1) {
			info.julia_param_1 = julia_param_1;
			info.julia_param_2 = julia_param_2;
		}

		//Dynamic allocation of the color matrix and the helper array
		help_array = new int[width + 1];
		color_matrix = new int*[height];
		for(int i = 0; i < height; i++) {
			color_matrix[i] = new int[width];
		}

		//Compute the number of lines each worker has to compute
		lines_to_compute = height / size;
		lines_left = height % size;		//See if you can get rid of this !
		row = 0;	//the row from the matrix from which the computation starts

		//If there are more than one process,
		//send to each one the information to compute
		if(size > 1) {
			for (int i = 1; i < size; i++) {
				info.row = row;
				info.lines_to_compute = lines_to_compute;
				MPI_Send(&info, info_size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
				row += lines_to_compute;
			}
		}

		//The master process starts computing and populates the matrix of colors
		if(set_type == 0) {
			for(int i = row; i < height; i++) {
				for(int j = 0; j < width; j++) {
					color_matrix[i][j] = compute_Mandelbrot(i, j, &info);
				}
			}
		} else {
			for(int i = row; i < height; i++) {
				for(int j = 0; j < width; j++) {
					color_matrix[i][j] = compute_Julia(i, j, &info);
				}
			}
		}

		//If there are more than one process, receive from each one of them
		//the result of their computing and populate the matrix of colors
		if(size > 1) {
			for(int i = 0; i < row; i++) {
				MPI_Recv(help_array, width + 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
				for(int j = 1; j < width + 1; j++) {
					color_matrix[help_array[0]][j - 1] = help_array[j];
				}
			}
		}

		//The master process writes into the output file
		out << "P2\n";
		out << width << " " << height << "\n";
		out << NO_COLORS - 1 << "\n";
		for(int i = 0; i < height; i++) {
			for(int j = 0; j < width; j++) {
				out << color_matrix[height - i - 1][j] << " ";
			}
			out << "\n";
		}
	} else {	//Workers

		//The worker receives the information from the master
		MPI_Recv(&info, info_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
		help_array = new int[info.width + 1];

		//Depending of which algorithm it should apply, it sends back to the master
		//the result of it's computation
		if(info.set_type == 0) {
			for(int i = info.row; i < info.row + info.lines_to_compute; i++) {
				//Remember the line, so that the master will know where
				//in the matrix of colors should put the result
				help_array[0] = i;
				//Populates the helper array with the result of the algorithm
				for(int j = 0; j < info.width; j++) {
					help_array[j + 1] = compute_Mandelbrot(i, j, &info);
				}
				//Sends the result to the master
				MPI_Send(help_array, info.width + 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
			}
		} else {
			for(int i = info.row; i < info.row + info.lines_to_compute; i++) {
				//Remember the line, so that the master will know where
				//in the matrix of colors should put the result
				help_array[0] = i;
				//Populates the helper array with the result of the algorithm
				for(int j = 0; j < info.width; j++) {
					help_array[j + 1] = compute_Julia(i, j, &info);
				}
				//Sends the result to the master
				MPI_Send(help_array, info.width + 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
			}
		}
	}

	//Destroy the MPI World
	MPI_Finalize();
	return 0;
}

//Function that returns an integer calculated with the Mandelbrot algorithm
int compute_Mandelbrot(int i, int j, Info *info) {
	int count = 0;
	Complex z, c;
	z.re = z.im = 0.0;
	c.re = info->x_min + j * info->step;
	c.im = info->y_min + i * info->step;
	while(count < info->max_steps && magnitude(z) < 2) {
		z = mul_complex(z, z);
		z = add_complex(z, c);
		count++;
	}
	return count % NO_COLORS;
}

//Function that returns an integer calculated with the Julia algorithm
int compute_Julia(int i, int j, Info *info) {
	int count = 0;
	Complex z, c;
	c.re = info->julia_param_1;
	c.im = info->julia_param_2;
	z.re = info->x_min + j * info->step;
	z.im = info->y_min + i * info->step;
	while(count < info->max_steps && magnitude(z) < 2) {
		z = mul_complex(z, z);
		z = add_complex(z, c);
		count++;
	}
	return count % NO_COLORS;
}
