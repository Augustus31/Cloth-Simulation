#pragma once
#include <vector>
#include <Triangle.h>
#include <Particle.h>
#include <SpringDamper.h>

class Cloth {
public:
	void InitSimpleCloth();
	void Update(float deltaTime);
	void Draw(const glm::mat4& viewProjMtx, GLuint shader);
	int GetDimensionX();
	int GetDimensionZ();
	void SetFixed(int index);
	void UnsetFixed(int index);
	void MoveFixed(glm::vec3 dir);
	~Cloth();

private:
	int lengthx;
	int depthz;
	float scale;
	float ground;

	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	glm::mat4 model;
	glm::vec3 color;

	// Cube Information
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int>trindices; //for rendering

	std::vector<Particle*> particles;
	std::vector<Triangle*> triangles;
	std::vector<SpringDamper*> spdamps;
};