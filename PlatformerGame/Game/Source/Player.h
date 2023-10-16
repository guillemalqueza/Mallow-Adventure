#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "Physics.h"
#include "Animation.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:
	float speed = 0.2f;
	const char* texturePath;
	SDL_Texture* texture = NULL;
	PhysBody* pbody;
	int pickCoinFxId;

	Animation* currentAnim;
	Animation idleAnim, jumpAnim, walkAnim, crouchAnim, crouchWalkAnim, fallAnim, wallAnim, deadAnim;

	bool isJumping = false;
	bool isCrouching = false;
	bool isWalking = false;
	bool ground = true;
	bool isFacingRight = true;
	bool isFacingUp = false;
	int jumpCount = 0;
	int dashCount = 0;
	bool wall = false;
	float previousY = 0.0f;
	bool hasJumped = false;
	bool isRunning = false;
	bool isDashing = false;
	bool canDash = true;
	bool jumper = false;
	bool isDead = false;

	bool lastKeyPressA = false;
	bool lastKeyPressD = false;

	float dashDuration = 2.0f;
	float dashVelocityX = 12.0f;
	float dashVelocityY = -4.0f;
	float dashTimer = 0.0f;
	
	int keys = 0;

};

#endif // __PLAYER_H__