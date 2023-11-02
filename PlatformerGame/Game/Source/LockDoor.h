#ifndef __LOCKDOOR_H__
#define __LOCKDOOR_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Timer.h"

struct SDL_Texture;

class LockDoor : public Entity
{
public:

	LockDoor();
	virtual ~LockDoor();

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
	Animation doorIdleAnim, doorOpenAnim;
};

#endif // __LOCKDOOR_H__
