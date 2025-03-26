#include <Triangle.h>
#include <Particle.h>
#include <iostream>

extern glm::vec3 wind;

Triangle::Triangle(int a, int b, int c, Particle* pa, Particle* pb, Particle* pc) {
	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);
	particles.push_back(pa);
	particles.push_back(pb);
	particles.push_back(pc);
}

void Triangle::Update() {
	glm::vec3 rawnormal = glm::cross(particles[2]->GetPosition() - particles[0]->GetPosition(), particles[1]->GetPosition() - particles[0]->GetPosition());
	glm::vec3 normal = glm::normalize(rawnormal);
	for (int i = 0; i < 3; i++) {
		particles[i]->AddNormal(normal);
	}
	this->normal = normal;
}

void Triangle::ApplyDrag() {
	glm::vec3 vel = (particles[0]->GetVelocity() + particles[1]->GetVelocity() + particles[2]->GetVelocity()) / 3.0f - wind;
	if (length(vel) > 0.001) {
		float a0 = 0.5f * glm::length(glm::cross(particles[2]->GetPosition() - particles[0]->GetPosition(), particles[1]->GetPosition() - particles[0]->GetPosition()));
		float a = a0 * glm::dot(glm::normalize(vel), normal);
		glm::vec3 f = -0.5f * 10.0f * 1.225f * 1.28f * glm::length(vel) * glm::length(vel) * a * normal;
		for (int i = 0; i < 3; i++) {
			particles[i]->ApplyForce(f / 3.0f);
		}

		//Debug
		//std::cout << "particle velocity" << std::endl;
		//std::cout << vel.x << " " << vel.y << " " << vel.z << std::endl;

		//std::cout << "cross sectional area" << std::endl;
		//std::cout << a << std::endl;

		//std::cout << "force" << std::endl;
		//std::cout << f.x << " " << f.y << " " << f.z << std::endl;

		//std::cout << "---------------------------------" << std::endl;
	}
}