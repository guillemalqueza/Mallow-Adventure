#ifndef __CRUMBLINGPLATFORM_H__
#define __CRUMBLINGPLATFORM_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Timer.h"

struct SDL_Texture;

class CrumblingPlatform : public Entity
{
public:

	CrumblingPlatform();
	virtual ~CrumblingPlatform();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	bool isActivated = false;

private:

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;
	bool isVisible = true;
	Timer timer;
	bool isTimerStarted = false;

	Animation* currentAnim;
	Animation platformIdleAnim, platformBreakingAnim;
};

#endif // __CRUMBLINGPLATFORM_H__
