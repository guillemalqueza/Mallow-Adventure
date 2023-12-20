#ifndef __KEY_H__
#define __KEY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Key : public Entity
{
public:

	Key();
	virtual ~Key();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnPicked();

public:

	bool isPicked = false;
	float followTimer = 0.0f;
	float acceleration = 1.0f;


private:

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;

	Animation* currentAnim;
	Animation keyIdleAnim;
};

#endif // __KEY_H__