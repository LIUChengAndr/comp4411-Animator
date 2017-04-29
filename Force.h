#ifndef FORCE_H
#define FORCE_H

#include "vec.h"
#include <vector>
#include <map>

class Particle;

class Force {
public:
	virtual void addForce(Particle* particle) = 0;
};

class Gravity : public Force {
public:
	Gravity(Vec3d v) : g(v) {}
	Vec3d g;   //gravity
	virtual void addForce(Particle* particle);
};

class Viscous : public Force {
public:
	Viscous(double m) : K(m) {}
	double K;  // k of the force
	virtual void addForce(Particle* particle);
};

#endif