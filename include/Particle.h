#pragma once
#include "core.h"

class Particle {
public:
	void ApplyForce(glm::vec3& f);
	void Integrate(float deltaTime);
	void AddNormal(glm::vec3 n);
	void NormalizeNormal();
	void ClearNormal();
	glm::vec3 GetPosition();
	glm::vec3 GetVelocity();
	glm::vec3 GetNormal();
	float GetMass();
	void GroundCheck(float ground);
	void move(glm::vec3 dir);
	Particle(float mass, glm::vec3 position);
	bool fixed;
private:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 force;
	glm::vec3 normal;
	float mass;
};
