/*
 * Student: Mardaloescu Serban
 * Grupa: 334CA
 */

#include <math.h>
class Complex {
public:
	double re;
	double im;
};

//Add two complex numbers
Complex add_complex(Complex c1, Complex c2);

//Multiply two complex numbers
Complex mul_complex(Complex c1, Complex c2);

//Calculate the magnitude of a complex number
double magnitude(Complex c);
