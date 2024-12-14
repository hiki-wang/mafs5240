#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <cassert>
#include "MathLib.h"

bool RandGen::seed = false;

RandGen::RandGen() {
	/* initialize uniform random number generator */
	if (!seed) {
		srand ( (unsigned int)time(NULL) );
		seed = true;
	}
}

Uniform* Uniform::_instance = 0;

Uniform*
Uniform::Instance() {
	if (_instance == 0) {
		_instance = new Uniform;
	}
	return _instance;
}

double Uniform::generate()
{
	double rv;
	do {
		rv = 1.0*rand()/RAND_MAX;
	} while (rv <= 0.0 || rv >= 1.0);
	return rv;
}

double __stdcall uniform() { return Uniform::Instance()->generate(); }


// Exponential subclass implementation
Exponential* Exponential::_instance = 0;

Exponential* Exponential::Instance() {
	if (_instance == 0) {
		_instance = new Exponential;
	}
	return _instance;
}

double Exponential::generate() {
	double u = Uniform::Instance()->generate();  // Get uniform random number
	return -log(u);  // Generate exponential random variate by inverse transform
}

double __stdcall exponential() {
	return Exponential::Instance()->generate();
}

// PolarRej subclass implementation
PolarRej* PolarRej::_instance = 0;

PolarRej* PolarRej::Instance() {
	if (_instance == 0) {
		_instance = new PolarRej;
	}
	return _instance;
}

double PolarRej::generate() {
	if (hasSpare) {
		hasSpare = false;
		return spare;  // Return the spare normal variate
	} else {
		double u, v, s;
		do {
			u = 2 * Uniform::Instance()->generate() - 1; // Generate uniform in (-1,1)
			v = 2 * Uniform::Instance()->generate() - 1;
			s = u * u + v * v;
		} while (s >= 1 || s == 0); // Reject if s is out of range
		double fac = sqrt(-2 * log(s) / s);  // Calculate the scaling factor
		double x = u * fac;
		spare = v * fac;  // Store the spare normal variate
		hasSpare = true;
		return x;  // Return the first normal variate
	}
}

double __stdcall normalPR() {
	return PolarRej::Instance()->generate();
}