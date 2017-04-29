 // SAMPLE_SOLUTION
#include "particle.h"
#include "modelerdraw.h"
#include <FL/gl.h>
#include <FL/glut.h>
#include <GL/glu.h>
#include <cstdio>
#include <math.h>
#include "Force.h"

void Particle::add_force(Force* f) {
	forces.push_back(f);
}

void Particle::nextPos(float deltaT) {
	for (std::vector<Force*>::iterator it = forces.begin(); it != forces.end(); it++)
	{
		(*it)->addForce(this);
	}
	speed += netForce/mass * deltaT;
	position += speed * deltaT;
}

void Particle::draw() {
	setDiffuseColor(0,0,1);
	glPushMatrix();
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex3f(position[0], position[1], position[2]);
	glEnd();
	glPopMatrix();
	glPointSize(1);
}