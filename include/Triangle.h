#pragma once
#include <vector>
#include <Particle.h>

class Triangle {
public:
	Triangle(int a, int b, int c, Particle* pa, Particle* pb, Particle* pc);
	void Update();
	void ApplyDrag();
private:
	glm::vec3 normal;
	std::vector<int> indices;
	std::vector<Particle*> particles;
};