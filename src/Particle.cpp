#include "Particle.h"
#include <iostream>

Particle::Particle(float mass, glm::vec3 position) {
	this->mass = mass;
	this->position = position;
	this->velocity = glm::vec3(0);
	this->force = glm::vec3(0);
	this->fixed = false;
}

void Particle::AddNormal(glm::vec3 n) {
	normal += n;
}
void Particle::NormalizeNormal() {
	normal = glm::normalize(normal);
}
void Particle::ClearNormal() {
	normal = glm::vec3(0);
}

void Particle::ApplyForce(glm::vec3& f) { force += f; }

void Particle::Integrate(float deltaTime) {
	if (!fixed) {
		glm::vec3 accel = force / mass; // Apply Newton’s Second Law (f=ma)
		velocity += accel * deltaTime; // Forward Euler integration to get new velocity
		position += velocity * deltaTime; // Backward Euler integration to get new position
		force = glm::vec3(0); // Zero force out so next frame will start fresh
	}
	else {
		force = glm::vec3(0); // Zero force out so next frame will start fresh
	}
	
}

glm::vec3 Particle::GetPosition() {
	return position;
}

glm::vec3 Particle::GetVelocity() {
	return velocity;
}

glm::vec3 Particle::GetNormal() {
	return normal;
}

float Particle::GetMass() {
	return mass;
}

void Particle::GroundCheck(float ground) {
	if (position.y < ground) {
		position.y = ground;
		glm::vec3 groundNormal = glm::vec3(0.0, 1.0, 0.0);
		velocity = velocity - glm::dot(velocity, groundNormal) * groundNormal;

		//friction simulation
		velocity = velocity * 0.99f;

		if (glm::length(velocity) < 0.01) {
			velocity = glm::vec3(0);
		}
	}
}

void Particle::move(glm::vec3 dir) {
	position += dir;
}
