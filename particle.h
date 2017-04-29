 // SAMPLE_SOLUTION
#ifndef PARTICLE_H
#define PARTICLE_H

#include "vec.h"
#include <vector>
#include <map>

class Force;

class Particle {
public:
	Particle(Vec3d p, double m) : position(p), mass(m) {}

	inline void setPos(Vec3d p) { position = p; }
	inline void setSpeed(Vec3d s) {	speed = s; }
	inline void setNetForce(Vec3d f) { netForce = f; }
	inline Vec3d getPos() const { return position; }
	inline Vec3d getSpeed() const { return speed; }
	inline Vec3d getNetForce() const { return netForce; }
	inline double getMass() const { return mass; }

	void add_force(Force* f);
	void nextPos(float deltaT);
	void draw();

private:
	double mass;
	Vec3d position;
	Vec3d speed;
	Vec3d netForce;
	std::vector<Force*> forces;
};

#endif