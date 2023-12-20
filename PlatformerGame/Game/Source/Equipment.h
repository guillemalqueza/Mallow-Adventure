#ifndef __EQUIPMENT_H__
#define __EQUIPMENT_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Equipment : public Entity
{
public:

	Equipment();
	virtual ~Equipment();

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
	bool soundPlayed = false;
	float followTimer = 0.0f;
	float acceleration = 1.0f;
	int equipmentReleaseFxId;
	int equipmentPickUpFxId;

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

#endif // __EQUIPMENT_H__