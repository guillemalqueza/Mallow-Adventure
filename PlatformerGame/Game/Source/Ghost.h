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

	void Move(const iPoint& origin, const iPoint& destination);

public:
	float speed = 0.2f;
	bool isActivated = false;
	bool isFacingRight = false;
	bool isSummonFacingRight = false;
	bool isSummonFollowing = false;
	int ghostSummonFxId;

	iPoint originPos;
	b2Vec2 velocity;

	iPoint summonOriginPos;
	b2Vec2 summonVelocity;

private:

	Animation* currentAnim;
	Animation* currentSummonAnim;

	Animation ghostAttackAnim, ghostDeadAnim, ghostIdleAnim, ghostIdle2Anim, ghosSkillAnim, ghostSummonAnim;
	Animation ghostSummonIdleAnim, ghostSummonAppearAnim, ghostSummonDeathAnim;

	SDL_Texture* texture;
	SDL_Texture* lightTexture;
	SDL_Texture* pathTexture;
	const char* texturePath;
	const char* lightTexturePath;;
	PhysBody* pbody;
	PhysBody* summonPbody;
	b2Transform initialTransform, initialSummonTransform;
	iPoint summonPosition;

	iPoint playerTilePos;
	iPoint summonTilePos;
	iPoint nextTilePath;

	float distance;
};

#endif // __GHOST_H__
