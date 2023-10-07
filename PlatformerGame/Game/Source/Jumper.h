#ifndef __JUMPER_H__
#define __JUMPER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Jumper : public Entity
{
public:

	Jumper();
	virtual ~Jumper();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	bool isActivated = false;

private:

	Animation* currentAnim;
	Animation idleAnim, jumpAnim;

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;
};

#endif // __JUMPER_H__
