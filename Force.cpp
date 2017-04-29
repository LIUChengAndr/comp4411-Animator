#include "Force.h"
#include "particle.h"

void Gravity::addForce(Particle* particle)
{
	particle->setNetForce(particle->getNetForce() + g * particle->getMass());
}

void Viscous::addForce(Particle* particle)
{
	particle->setNetForce(particle->getNetForce() - K * particle->getSpeed());
}