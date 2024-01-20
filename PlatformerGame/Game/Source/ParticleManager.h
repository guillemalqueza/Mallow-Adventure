#ifndef __PARTICLEMANAGER_H__
#define __PARTICLEMANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "List.h"
#include <vector>

const int PARTICLE_COUNT = 100;

struct Particle {
	int x, y;
	int radius;
	int speed;
	int timeOffset;
	float waveSpeed;
};

class ParticleManager : public Module
{
public:

	ParticleManager(bool enabled = true);

	// Destructor
	virtual ~ParticleManager();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Additional methods
	void CreateParticles();
	void UpdateParticles(float dt, float time);
	void DrawParticles();

public:

	List<Entity*> entities;
	std::vector<Particle> particles;
	SDL_Texture* particleTexture1;
	SDL_Texture* particleTexture2;
	SDL_Texture* particleTexture3;
};

#endif // __PARTICLEMANAGER_H__
