#pragma once
#include <Particle.h>
class SpringDamper {
public:
	SpringDamper(Particle* p1, Particle* p2, float s, float d, float sr);
	Particle* p1;
	Particle* p2;
	void ApplyForces();
private:
	float springConstant;
	float dampingConstant;
	float springRest;
};