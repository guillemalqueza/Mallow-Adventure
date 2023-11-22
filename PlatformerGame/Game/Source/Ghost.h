#ifndef __GHOST_H__
#define __GHOST_H__

#include "Entity.h"
#include "Point.h"
#include "Physics.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Ghost : public Entity
{
public:

	Ghost();
	virtual ~Ghost();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void LoadAnimations();

public:
	float speed = 0.2f;
	bool isActivated = false;

private:

	Animation* currentAnim;
	Animation ghostAttackAnim, ghostDeadAnim, ghostIdleAnim, ghostIdle2Anim, ghosSkillAnim;

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;
	b2Transform initialTransform;
};

#endif // __GHOST_H__
