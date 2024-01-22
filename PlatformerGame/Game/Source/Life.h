#ifndef __LIFE_H__
#define __LIFE_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Life : public Entity
{
public:

	Life();
	virtual ~Life();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	bool isPicked = false;
	bool lifeArea = false;
	bool lifeIdleWaiting = false;
	float followTimer = 0.0f;
	float acceleration = 1.0f;
	int lifeAudioFxId;

private:

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;

	Animation* currentAnim;
	Animation lifePotionAnim;
};

#endif // __LIFE_H__