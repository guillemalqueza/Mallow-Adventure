#ifndef __GHOST_H__
#define __GHOST_H__

#include "Entity.h"
#include "Point.h"
#include "Physics.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Point.h"
#include "DynArray.h"

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
	SDL_Texture* bigLightTexture;
	SDL_Texture* pathTexture;
	SDL_Texture* healthBarTexture;
	const char* texturePath;
	const char* lightTexturePath;
	const char* bigLightTexturePath;
	PhysBody* pbody;
	PhysBody* summonPbody;
	PhysBody* enemySummonPbody;
	b2Transform initialTransform, initialSummonTransform;
	iPoint summonPosition;

	iPoint playerTilePos;
	iPoint summonTilePos;
	iPoint nextTilePath;

	float distance;
	bool summonSpawned = false;

	iPoint initialIdlePosition;

	const DynArray<iPoint>* path;

	SDL_Rect healthBarRect100 = { 0,0,80,32 };
	SDL_Rect healthBarRect50 = { 0,32,80,32 };
	SDL_Rect healthBarRect0 = { 0,96,80,32 };
};

#endif // __GHOST_H__
