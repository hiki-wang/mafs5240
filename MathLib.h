#ifndef MATHLIB_H
#define MATHLIB_H
#include <cmath>

class RandGen {
private:
	static bool seed;
public:
	RandGen();
	virtual double generate() = 0;
};

class Uniform : public RandGen {
private:
	static Uniform* _instance;
	Uniform() {}
public:
	static Uniform* Instance();
	double generate();
};

class Exponential : public RandGen {
private:
	static Exponential* _instance;
	Exponential() {}
public:
	static Exponential* Instance();
	double generate();
};

class PolarRej : public RandGen {
private:
	static PolarRej* _instance;
	double spare;  // Store the spare normal variate
	bool hasSpare;  // Flag to indicate if spare is available
	PolarRej() : hasSpare(false) {}
public:
	static PolarRej* Instance();
	double generate();
};

double __stdcall uniform();
double __stdcall exponential();
double __stdcall normalPR();

#endif