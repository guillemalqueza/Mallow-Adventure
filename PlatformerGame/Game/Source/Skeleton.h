#ifndef __SKELETON_H__
#define __SKELETON_H__

#include "Entity.h"
#include "Point.h"
#include "Physics.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Skeleton : public Entity
{
public:

	Skeleton();
	virtual ~Skeleton();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void LoadAnimations();

	void Move(const iPoint& origin, const iPoint& destination);

public:
	float speed = 0.2f;
	bool isActivated = false;
	bool isFacingRight = false;
	iPoint originPos;
	b2Vec2 velocity;

private:

	Animation* currentAnim;
	Animation skeletonIdleAnim, skeletonAttackAnim, skeletonDeadAnim, skeletonWalkAnim, skeletonHurtAnim;

	SDL_Texture* texture;
	SDL_Texture* pathTexture;
	const char* texturePath;
	PhysBody* pbody;
	b2Transform initialTransform;
};

#endif // __SKELETON_H__
