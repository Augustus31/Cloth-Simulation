#include "SpringDamper.h"

SpringDamper::SpringDamper(Particle* p1, Particle* p2, float s, float d, float sr) {
	this->p1 = p1;
	this->p2 = p2;
	this->springConstant = s;
	this->dampingConstant = d;
	this->springRest = sr;
}

void SpringDamper::ApplyForces() {

	if (glm::length(p2->GetPosition() - p1->GetPosition()) > 0.05f) {
		//Spring
		float x = glm::length(p2->GetPosition() - p1->GetPosition()) - springRest;
		glm::vec3 e1 = (p2->GetPosition() - p1->GetPosition()) / glm::length(p2->GetPosition() - p1->GetPosition());
		glm::vec3 e2 = -1.0f * e1;
		glm::vec3 springforce1 = springConstant * x * e1;
		glm::vec3 springforce2 = springConstant * x * e2;
		p1->ApplyForce(springforce1);
		p2->ApplyForce(springforce2);

		//Damper
		float vclose = glm::dot((p1->GetVelocity() - p2->GetVelocity()), e1);
		glm::vec3 dampingforce1 = -1.0f * vclose * dampingConstant * e1;
		glm::vec3 dampingforce2 = -1.0f * vclose * dampingConstant * e2;
		p1->ApplyForce(dampingforce1);
		p2->ApplyForce(dampingforce2);
	}
}