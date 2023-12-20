#ifndef __LOGOBSTACLE_H__
#define __LOGOBSTACLE_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Physics.h"

struct SDL_Texture;

class LogObstacle : public Entity
{
public:

	LogObstacle();
	virtual ~LogObstacle();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);


public:

	bool isPicked = false;
	float followTimer = 0.0f;
	float acceleration = 1.0f;
	int health = 4;
	bool logUp = true;
	bool changeLevel = false;
	int logAudioKnock1FxId;
	int logAudioKnock2FxId;
	int logAudioKnock3FxId;
	int logAudioBreakFxId;


private:

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;

	Animation* currentAnim;
	Animation logIdleAnim;
	Animation logDownAnim;
	Animation logDownIdle;

	b2Vec2 initialPosition;
};

#endif // __LOGOBSTACLE_H__