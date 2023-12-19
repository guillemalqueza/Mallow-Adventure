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

	void SetToInitialPosition();

	void LoadAnimations();
	
	void ToggleGodMode();

	void DrawPlayer();

	void LeftMovement();

	void RightMovement();

	void Jump();

	void ClimbUp();

	void ClimbDown();

public:
	float speed = 0.2f;
	const char* texturePath;
	const char* lightTexturePath;
	const char* effectsTexturePath;
	SDL_Texture* texture = NULL;
	SDL_Texture* effectsTexture = NULL;
	SDL_Texture* lightTexture;
	PhysBody* pbody;
	PhysBody* pbodySword = NULL;
	int pickCoinFxId;
	int swordAudio1FxId;
	int swordAudio2FxId;
	int swordAudio3FxId;
	int swordAudio4FxId;
	int jumpAudio1FxId;
	int jumpAudio2FxId;
	int jumpAudio3FxId;
	int jumpAudio4FxId;
	int potionDrinkFxId;
	int land1FxId;
	int land2FxId;
	int deathAudioFxId;

	Animation* currentAnim;
	Animation idleAnim, jumpAnim, walkAnim, crouchAnim, crouchWalkAnim, fallAnim, wallAnim, deadAnim,landJumpAnim,
				armorIdleAnim, armorJumpAnim, armorWalkAnim, armorCrouchAnim, armorCrouchWalkAnim, armorFallAnim, armorDeadAnim, 
				attack1Anim, attack2Anim, attack3Anim, attackJumpAnim, pushAnim, swordAnim, doorAnim, drinkAnim, armorLandJumpAnim;

	Animation* currentEffectsAnim;
	Animation climbEffectAnim, jumpEffectAnim, dangerEffectAnim;

	bool isJumping = false;
	bool isCrouching = false;
	bool isWalking = false;
	bool isPushing = false;
	bool ground = true;
	bool isFacingRight = true;
	bool isFacingUp = false;
	int jumpCount = 0;
	int dashCount = 0;
	bool wallLeft = false;
	bool wallRight = false;
	bool wallEnd = false;
	float previousY = 0.0f;
	bool hasJumped = false;
	bool isLanding = false;
	bool isRunning = false;
	bool isDashing = false;
	bool canDash = true;
	bool jumper = false;
	bool isDead = false;
	bool isEquipped = false;
	bool isAttacking = false;
	bool firstAttack = true;
	bool attackBodyCreated = false;
	bool destroyAttackBody = false;
	bool activeSword = false;
	bool enterDoor = false;
	bool isDrinking = false;
	bool canPush = false;
	bool canOpen = false;

	bool lastKeyPressA = false;
	bool lastKeyPressD = false;

	float dashDuration = 2.0f;
	float dashVelocityX = 12.0f;
	float dashVelocityY = -6.0f;
	float dashTimer = 0.0f;
	
	int keys = 0;
	bool godMode = false;

	b2Transform initialTransform;

	b2Vec2 vel;

	float dt;

};

#endif // __PLAYER_H__