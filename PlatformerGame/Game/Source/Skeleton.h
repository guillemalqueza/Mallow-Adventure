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
	bool open = false;
	bool isActivated = false;
	bool isFacingRight = false;
	iPoint originPos;
	b2Vec2 velocity;
	int attackAudio1FxId;
	int attackAudio2FxId;
	int attackAudio3FxId;
	int hurtAudioFxId;
	int deathAudioFxId;


private:

	Animation* currentAnim;
	Animation skeletonIdleAnim, skeletonAttackAnim, skeletonDeadAnim, skeletonWalkAnim, skeletonHurtAnim;

	SDL_Texture* texture;
	SDL_Texture* pathTexture;

	const char* texturePath;
	PhysBody* pbody;
	PhysBody* enemyPbody;
	b2Transform initialTransform;

	iPoint playerTilePos;
	iPoint skeletonTilePos;
	iPoint nextTilePath;

	PhysBody* pbodySword = NULL;
	bool isAttacking = false;
	bool attackBodyCreated = false;
	bool destroyAttackBody = false;

	float distance;
	int health = 100;
	bool isDead = false;

	int initialIdlePosition;
};

#endif // __SKELETON_H__
