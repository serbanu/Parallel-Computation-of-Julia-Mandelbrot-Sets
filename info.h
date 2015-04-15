/*
 * Student: Mardaloescu Serban
 * Grupa: 334CA
 */

class Info {
public:
	double x_min;			//The lower limit on the X-axis
	double y_min;			//The lower limit on the Y-axis
	double step;			//Resolution
	int max_steps;			//Maximum number of iterations
	int width;				//Width of the .pgm matrix
	int height;				//Height of the .pgm matrix
	int set_type;			//Type of the set
	double julia_param_1;	//#1 Julia parameter
	double julia_param_2;	//#2 Julia parameter
	int row;				//The row from wich the processing begins
	int lines_to_compute;	//Number of lines to process
};
