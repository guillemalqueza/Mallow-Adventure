#include "ParticleManager.h"
#include "App.h"
#include "Textures.h"

#include "Defs.h"
#include "Log.h"


ParticleManager::ParticleManager() : Module()
{
	name.Create("particlemanager");
}

// Destructor
ParticleManager::~ParticleManager()
{}

// Called before render is available
bool ParticleManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Particle Manager");
	bool ret = true;

	return ret;
}

bool ParticleManager::Start() {

	bool ret = true;

	particleTexture1 = app->tex->Load("Assets/Textures/snow1.png");
	particleTexture2 = app->tex->Load("Assets/Textures/snow2.png");
	particleTexture3 = app->tex->Load("Assets/Textures/snow3.png");

	CreateParticles();

	return ret;
}

// Called before quitting
bool ParticleManager::CleanUp()
{
	bool ret = true;

	return ret;
}

bool ParticleManager::Update(float dt)
{
	bool ret = true;

	float time = SDL_GetTicks() / 1000.0f;

	UpdateParticles(dt, time);
	DrawParticles();

	return ret;
}

void ParticleManager::CreateParticles() {
	particles.clear();
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		Particle particle;
		particle.x = rand() % (1600 + 200) - 100;
		particle.y = rand() % (900 + 200) - 100;
		particle.radius = rand() % 3 + 1;
		particle.speed = rand() % 1 + 1;
		particle.timeOffset = rand() % 8 - 4;
		particle.waveSpeed = 0.6f + (rand() % 31 - 15) / 20.0f;
		particles.push_back(particle);
	}
}

void ParticleManager::UpdateParticles(float dt, float time) {
	for (auto& particle : particles) {
		particle.x -= particle.speed * dt;

		if (particle.x < -100) {
			particle.x = 1600 + 100;
			if (particle.y < -100 || particle.y > 900 + 100) {
				particle.y = rand() % (900 - 200) + 100;
			}
		}

		particle.y += sin(time + particle.timeOffset) * particle.waveSpeed * 0.6f * dt;
	}
}

void ParticleManager::DrawParticles() {
	for (auto& particle : particles) {
		SDL_Rect particleRect = { particle.x, particle.y, particle.radius * 2, particle.radius * 2 };
		if (particle.radius == 1) app->render->DrawTexture(particleTexture1, (-app->render->camera.x) + particleRect.x, (-app->render->camera.y) + particleRect.y);
		else if (particle.radius == 2) app->render->DrawTexture(particleTexture2, (-app->render->camera.x) + particleRect.x, (-app->render->camera.y) + particleRect.y);
		else if (particle.radius == 3) app->render->DrawTexture(particleTexture3, (-app->render->camera.x) + particleRect.x, (-app->render->camera.y) + particleRect.y);
	}
}