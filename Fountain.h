#pragma once

#include "ps.h"
#include "texture.h"
#include <iostream>
#include <ctime>
#include <math.h>


namespace Fountain {

	#define PARTICLE_TYPE_LAUNCHER 0.0f
	#define PARTICLE_TYPE_SHELL 1.0f
	#define MAX_VELOC glm::vec3(0.0,-3.0,0.0)
	#define MIN_VELOC glm::vec3(0.0,-1.0,0.0)
	#define MAX_LAUNCH 2.0f*1000.0f
	#define MIN_LAUNCH 0.3f*1000.0f
	#define INIT_SIZE 10.0f
	#define MAX_SIZE 8.0f
	#define MIN_SIZE 3.0f

	const float ANGLE = 120.0f;
	const int MAX_PARTICLES = 10000;

	const int INIT_PARTICLES = 5000;
	const glm::vec3 center(0.0f);
	const float radius = 0.01f;

	struct WaterParticle
	{
		float type;
		glm::vec3 position;
		glm::vec3 velocity;
		float lifetimeMills;
		float size;
	};

	class Fountain
	{
	public:
		Fountain();
		~Fountain();
		void Render(float frametimeMills, glm::mat4& worldMatrix, glm::mat4 viewMatrix, glm::mat4& projectMatrix);
	private:
		bool InitFountain();
		void UpdateParticles(float frametimeMills);
		void InitRandomTexture(unsigned int size);
		void RenderParticles(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectMatrix);
		void GenInitLocation(WaterParticle partciles[], int nums);

		unsigned int mCurVBOIndex, mCurTransformFeedbackIndex;
		GLuint mParticleBuffers[2]; 
		GLuint mParticleArrays[2];
		GLuint mTransformFeedbacks[2];
		GLuint mRandomTexture;
		CTexture mSparkTexture;
		CTexture mStartTexture;
		float mTimer;
		bool mFirst;
		Ps* mUpdateShader;
		Ps* mRenderShader;//äÖÈ¾Á£×ÓµÄGPUProgram
	};

}
