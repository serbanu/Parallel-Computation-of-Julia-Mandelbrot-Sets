/*
 * Student: Mardaloescu Serban
 * Grupa: 334CA
 */

#include "complex.h"

Complex add_complex(Complex c1, Complex c2) {
	Complex res;
	res.re = c1.re + c2.re;
	res.im = c1.im + c2.im;
	return res;
}

Complex mul_complex(Complex c1, Complex c2) {
	Complex res;
	res.re = c1.re * c2.re - c1.im * c2.im;
	res.im = c1.re * c2.im + c1.im * c2.re;
	return res;
}

double magnitude(Complex c) {
	return sqrt(c.re * c.re + c.im * c.im);
}

