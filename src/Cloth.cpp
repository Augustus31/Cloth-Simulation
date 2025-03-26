#include <Cloth.h>
#include <iostream>


void Cloth::InitSimpleCloth() {

	lengthx = 21;
	depthz = 21;
	scale = 0.15;
	ground = -2.0;

	for (int i = 0; i < depthz; i++) {
		for (int j = 0; j < lengthx; j++) {
			Particle* p = new Particle(0.1f, glm::vec3(j*scale, 0, i*scale));
			particles.push_back(p);
		}
	}

	for (int i = 0; i < depthz - 1; i++) {
		for (int j = 0; j < lengthx - 1; j++) {
			int i0 = j + i * lengthx;
			int i1 = j + 1 + i * lengthx;
			int i2 = j + (i + 1) * lengthx;
			int i3 = j + 1 + (i + 1) * lengthx;

			Triangle* tri1 = new Triangle(i0, i2, i3, particles[i0], particles[i2], particles[i3]);
			Triangle* tri2 = new Triangle(i3, i1, i0, particles[i3], particles[i1], particles[i0]);

			triangles.push_back(tri1);
			triangles.push_back(tri2);

			trindices.push_back(i0);
			trindices.push_back(i2);
			trindices.push_back(i3);
			trindices.push_back(i3);
			trindices.push_back(i1);
			trindices.push_back(i0);

			float sconst = 350.0;
			float dconst = 2.5;

			SpringDamper* sd1 = new SpringDamper(particles[i0], particles[i3], sconst, dconst, glm::sqrt(2.0) * scale);
			SpringDamper* sd2 = new SpringDamper(particles[i1], particles[i2], sconst, dconst, glm::sqrt(2.0) * scale);
			SpringDamper* sd3 = new SpringDamper(particles[i0], particles[i1], sconst, dconst, scale);
			SpringDamper* sd4 = new SpringDamper(particles[i2], particles[i3], sconst, dconst, scale);
			SpringDamper* sd5 = new SpringDamper(particles[i0], particles[i2], sconst, dconst, scale);
			SpringDamper* sd6 = new SpringDamper(particles[i1], particles[i3], sconst, dconst, scale);
			spdamps.push_back(sd1);
			spdamps.push_back(sd2);
			spdamps.push_back(sd3);
			spdamps.push_back(sd4);
			spdamps.push_back(sd5);
			spdamps.push_back(sd6);

		}
	}

	for (int i = 0; i < depthz - 2; i++) {
		for (int j = 0; j < lengthx - 2; j++) {
			int i0 = j + i * lengthx;
			int i1 = j + 2 + i * lengthx;
			int i2 = j + (i + 2) * lengthx;
			int i3 = j + 2 + (i + 2) * lengthx;

			float sconst = 150.0;
			float dconst = 2.0;

			SpringDamper* sd1 = new SpringDamper(particles[i0], particles[i3], sconst, dconst, 2 * glm::sqrt(2.0) * scale);
			SpringDamper* sd2 = new SpringDamper(particles[i1], particles[i2], sconst, dconst, 2 * glm::sqrt(2.0) * scale);
			SpringDamper* sd3 = new SpringDamper(particles[i0], particles[i1], sconst, dconst, 2 * scale);
			SpringDamper* sd4 = new SpringDamper(particles[i2], particles[i3], sconst, dconst, 2 * scale);
			SpringDamper* sd5 = new SpringDamper(particles[i0], particles[i2], sconst, dconst, 2 * scale);
			SpringDamper* sd6 = new SpringDamper(particles[i1], particles[i3], sconst, dconst, 2 * scale);
			spdamps.push_back(sd1);
			spdamps.push_back(sd2);
			spdamps.push_back(sd3);
			spdamps.push_back(sd4);
			spdamps.push_back(sd5);
			spdamps.push_back(sd6);

		}
	}

	//particles[71]->fixed = true; //debug
	//particles[204]->fixed = true; //debug
	//particles[350]->fixed = true; //debug

	// Model matrix.
	model = glm::mat4(1.0f);

	// The color of the cube. Try setting it to something else!
	color = glm::vec3(0.7, 0.3, 0.3);

	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);
	glGenBuffers(1, &EBO);
}

void Cloth::Update(float deltaTime) {
	deltaTime = deltaTime / 1000.0; //convert from ms to s
	float oversampleRate = 10;
	float oversample = deltaTime / (float)oversampleRate;
	for (int j = 0; j < oversampleRate; j++) {
		//Apply gravity to each particle
		for (int i = 0; i < particles.size(); i++) {
			particles[i]->ApplyForce(glm::vec3(0, -9.8 * particles[i]->GetMass(), 0));
		}

		//Apply spring-damper forces
		for (int i = 0; i < spdamps.size(); i++) {
			spdamps[i]->ApplyForces();
		}

		//Update Normals
		for (int i = 0; i < particles.size(); i++) {
			particles[i]->ClearNormal();
		}

		for (int i = 0; i < triangles.size(); i++) {
			triangles[i]->Update();
		}

		for (int i = 0; i < particles.size(); i++) {
			particles[i]->NormalizeNormal();
		}

		//std::cout << "---------------------------------\n----------------------------------\n" << std::endl;
		//Apply aerodynamic drag
		for (int i = 0; i < triangles.size(); i++) {
			triangles[i]->ApplyDrag();
		}

		//Integrate to get new position
		//std::cout << deltaTime << std::endl;
		for (int i = 0; i < particles.size(); i++) {
			particles[i]->Integrate(oversample);
		}
		//std::cout << particles[0]->GetPosition().x << " " << particles[0]->GetPosition().y << " " << particles[0]->GetPosition().z << std::endl;

		for (int i = 0; i < particles.size(); i++) {
			particles[i]->GroundCheck(ground);
		}
	}

	
	//GL Render Code
	std::vector<glm::vec3> posArr;
	std::vector<glm::vec3> normArr;

	for (int i = 0; i < particles.size(); i++) {
		posArr.push_back(particles[i]->GetPosition());
		normArr.push_back(particles[i]->GetNormal());
	}

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * posArr.size(), posArr.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normArr.size(), normArr.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind the EBO to the bound VAO and send the data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * trindices.size(), trindices.data(), GL_STATIC_DRAW);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Cloth::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
	// actiavte the shader program
	glUseProgram(shader);

	// get the locations and send the uniforms to the shader
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, trindices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

int Cloth::GetDimensionX() {
	return lengthx;
}
int Cloth::GetDimensionZ() {
	return depthz;
}

void Cloth::SetFixed(int index) {
	particles[index]->fixed = true;
}
void Cloth::UnsetFixed(int index) {
	particles[index]->fixed = false;
}

void Cloth::MoveFixed(glm::vec3 dir) {
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i]->fixed) {
			particles[i]->move(dir);
		}
	}
}

Cloth::~Cloth() {
	// Delete the VBOs and the VAO.
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);

	//Clean up all objects created with new
	//TODO
}