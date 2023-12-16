#ifndef __CHEST_H__
#define __CHEST_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Chest : public Entity
{
public:

	Chest();
	virtual ~Chest();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnPicked();

	void OnSensor();

public:

	bool isPicked = false;
	bool equipmentArea = false;
	bool equipmentIdleWaiting = false;
	float followTimer = 0.0f;
	float acceleration = 1.0f;


private:

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;
	PhysBody* pbody2;

	Animation* currentAnim;
	Animation equipmentWithSword;
	Animation equipmentAnim;
	Animation equipmentWithoutSword;
	Animation equipmentAnimIdle;
};

#endif // __CHEST_H__