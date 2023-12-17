#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "FadeToBlack.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

}

Player::~Player() {

}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	lightTexturePath = parameters.attribute("lightTexture").as_string();
	effectsTexturePath = parameters.attribute("effectsTexture").as_string();
	speed = parameters.attribute("speed").as_float();
	pickCoinFxId = app->audio->LoadFx(parameters.child("itemAudio").attribute("path").as_string());
	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	lightTexture = app->tex->Load(lightTexturePath);
	effectsTexture = app->tex->Load(effectsTexturePath);
	LoadAnimations();

	if (!isEquipped) currentAnim = &idleAnim;
	else currentAnim = &armorIdleAnim;

	jumper = false;

	pbody = app->physics->CreateCircle(position.x + 50, position.y, 22, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	initialTransform = pbody->body->GetTransform();

	return true;
}

bool Player::Update(float dt)
{
	//restart position
	if (app->input->GetKey(SDL_SCANCODE_F3)) SetToInitialPosition();

	//godmode
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) ToggleGodMode();

	if (!isDead)
	{
		if (!isJumping && !isCrouching && !isAttacking && !activeSword && !enterDoor)
		{
			if (!isEquipped) currentAnim = &idleAnim;
			else currentAnim = &armorIdleAnim;
		}

		b2Vec2 vel = pbody->body->GetLinearVelocity();

		if (!godMode)
		{
			//idle wall keys
			if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_IDLE && !ground && (wallLeft || wallRight) && !activeSword)
			{
				pbody->body->SetGravityScale(0.0f);
				pbody->body->SetLinearVelocity({ pbody->body->GetLinearVelocity().x, 0 });
				vel = b2Vec2(0, 0);
				if (!wallEnd) currentAnim = &wallAnim;
			}
			else if (!wallLeft && !wallRight)
			{
				pbody->body->SetGravityScale(1.0f);
			}

			if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_IDLE) isFacingUp = false;

			//climb wall keys
			if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT && !activeSword && !enterDoor)
			{
				isFacingUp = true;

				if ((wallLeft || wallRight) && !wallEnd)
				{
					ground = false;
					vel = b2Vec2(0, -speed * dt);
					currentAnim = &wallAnim;
				}
				else if (wallEnd && wallRight)
				{
					vel = b2Vec2(speed * dt, 0);
					currentAnim = &walkAnim;
				}
				else if (wallEnd && wallLeft)
				{
					vel = b2Vec2(-speed * dt, 0);
					currentAnim = &walkAnim;
				}
			}
			if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && !activeSword && !enterDoor)
			{
				if ((wallRight) && wallEnd)
				{
					ground = false;
					vel = b2Vec2(-(speed/2) * dt, speed * dt);
					currentAnim = &wallAnim;
				}
				else if ((wallLeft) && wallEnd)
				{
					ground = false;
					vel = b2Vec2((speed/2) * dt, speed * dt);
					currentAnim = &wallAnim;
				}
				else
				{
					ground = false;
					vel = b2Vec2(0, speed * dt);
					currentAnim = &wallAnim;
				}
				if (wallRight) isFacingRight = true;
				else if (wallLeft) isFacingRight = false;
			}

			//crouch
			if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && !isJumping && !activeSword && !enterDoor)
			{
				if (!isCrouching)
				{
					isCrouching = true;
					pbody->body->GetFixtureList()->GetShape()->m_radius = 0.22f;
				}
				if (!isWalking)
				{
					if (!isEquipped) currentAnim = &crouchAnim;
					else currentAnim = &armorCrouchAnim;
				}
			}
			else if (isCrouching)
			{
				isCrouching = false;
				pbody->body->GetFixtureList()->GetShape()->m_radius = 0.44f;
			}

			//player movement
			if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && !activeSword && !enterDoor)
			{
				if (!isDashing) vel.x = -speed * dt;
				if (!isJumping && !wallLeft && !wallRight)
				{
					if (isCrouching)
					{
						if (!isEquipped) currentAnim = &crouchWalkAnim;
						else currentAnim = &armorCrouchWalkAnim;
					}
					else if (!isPushing)
					{
						if (!isEquipped) currentAnim = &walkAnim;
						else currentAnim = &armorWalkAnim;
					}
					else
					{
						currentAnim = &pushAnim;
					}
				}
				isWalking = true;
				if (currentAnim == &wallAnim && !ground && isFacingRight)
				{
					wallLeft = false;
					wallRight = false;
					if (!isEquipped) currentAnim = &idleAnim;
					else currentAnim = &armorIdleAnim;
					pbody->body->SetGravityScale(1.0f);
				}
				isFacingRight = false;
			}

			if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && !activeSword && !enterDoor)
			{
				if (!isDashing) vel.x = speed * dt;
				if (!isJumping && !wallLeft && !wallRight)
				{
					if (isCrouching)
					{
						if (!isEquipped) currentAnim = &crouchWalkAnim;
						else currentAnim = &armorCrouchWalkAnim;
					}
					else if (!isPushing)
					{
						if (!isEquipped) currentAnim = &walkAnim;
						else currentAnim = &armorWalkAnim;
					}
					else
					{
						currentAnim = &pushAnim;
					}
				}
				isWalking = true;
				if (currentAnim == &wallAnim && !ground && !isFacingRight)
				{
					wallLeft = false;
					wallRight = false;
					if (!isEquipped) currentAnim = &idleAnim;
					else currentAnim = &armorIdleAnim;
					pbody->body->SetGravityScale(1.0f);
				}
				isFacingRight = true;
			}

			if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_IDLE && !isDashing && !wallEnd && (!wallLeft || !wallRight))
			{
				if (isPushing) isPushing = false;
				isWalking = false;
				vel.x = 0;
			}
			else if (wallEnd && app->input->GetKey(SDL_SCANCODE_UP) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_IDLE
				&& ((wallRight && app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) || (wallLeft && app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)))
			{
				vel.x = 0;
			}

			//printf("\r jumpcount %d, isJumping %i, hasJumped %i, ground %i, dashCount %d", jumpCount, isJumping, hasJumped, ground, dashCount);
			//jump
			if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !activeSword && !enterDoor)
			{	
				isPushing = false;
				wallLeft = false;
				wallRight = false;

				if (!isJumping && !hasJumped && ground && jumpCount == 0)
				{
					currentAnim->Reset();
					if (currentEffectsAnim != nullptr) currentEffectsAnim->Reset();
					if (!isEquipped) currentAnim = &jumpAnim;
					else currentAnim = &armorJumpAnim;
					currentEffectsAnim = &jumpEffectAnim;
					isJumping = true;
					hasJumped = true;
					vel = b2Vec2(vel.x, -8.2f);
					pbody->body->SetLinearVelocity(vel);
					jumpCount = 1;
				}
				else if (jumpCount == 1 && hasJumped && dashCount == 0)
				{
					currentAnim->Reset();
					if (currentEffectsAnim != nullptr) currentEffectsAnim->Reset();
					if (!isEquipped) currentAnim = &jumpAnim;
					else currentAnim = &armorJumpAnim;
					currentEffectsAnim = &jumpEffectAnim;
					isJumping = true;
					vel = b2Vec2(vel.x, -8.2f);
					pbody->body->SetLinearVelocity(vel);
					jumpCount = 2;
					dashCount = 1;
				}
				else if (!ground && !isJumping && !hasJumped && jumpCount == 0 && dashCount == 0)
				{
					currentAnim->Reset();
					if (currentEffectsAnim != nullptr) currentEffectsAnim->Reset();
					if (!isEquipped) currentAnim = &jumpAnim;
					else currentAnim = &armorJumpAnim;
					currentEffectsAnim = &jumpEffectAnim;
					isJumping = true;
					vel = b2Vec2(vel.x, -8.2f);
					pbody->body->SetLinearVelocity(vel);
					jumpCount = 2;
					dashCount = 1;
				}

			}

			//dash
			if (app->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN && !ground && !isDashing && dashCount == 0) {
				isDashing = true;
				dashTimer = 0.0f;

				if (isFacingUp)
				{
					vel.y += (isFacingUp ? dashVelocityY : 0.0f);
				}
				else
				{
					vel.x += (isFacingRight ? dashVelocityX : -dashVelocityX);
					vel.y += (isFacingUp ? dashVelocityY : 0.0f);
				}

				dashCount = 1;
			}

			if (isDashing) {
				dashTimer += 0.1f;
				if (dashTimer >= dashDuration) {
					isDashing = false;
				}
			}

			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN && !isAttacking && isEquipped && !activeSword && !enterDoor)
			{
				isAttacking = true;
				
				if (isJumping) currentAnim = &attackJumpAnim;
				else if (firstAttack)
				{
					currentAnim = &attack1Anim;
					firstAttack = false;
				}
				else
				{
					currentAnim = &attack2Anim;
					firstAttack = true;
				}
	
				currentAnim->ResetLoopCount();
				currentAnim->Reset();
			}
			
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN && !isAttacking && isEquipped && !activeSword && !enterDoor)
			{
				isAttacking = true;

				currentAnim = &attack3Anim;
				currentAnim->ResetLoopCount();
				currentAnim->Reset();
			}

			if (destroyAttackBody)
			{
				pbodySword->body->GetWorld()->DestroyBody(pbodySword->body);
				pbodySword = NULL;
				destroyAttackBody = false;
			}

			if (isAttacking)
			{
				if ((currentAnim == &attack1Anim || currentAnim == &attack2Anim) && currentAnim->GetCurrentFrameCount() >= 1 && !attackBodyCreated)
				{
					if (isFacingRight) pbodySword = app->physics->CreateRectangleSensor(position.x + 90, position.y + 20, 25, 25, bodyType::STATIC);
					else pbodySword = app->physics->CreateRectangleSensor(position.x, position.y + 20, 25, 25, bodyType::STATIC);
					pbodySword->ctype = ColliderType::SWORD;
					attackBodyCreated = true;
				}
				else if (currentAnim == &attackJumpAnim && currentAnim->GetCurrentFrameCount() >= 2 && !attackBodyCreated)
				{
					pbodySword = app->physics->CreateRectangleSensor(position.x + 45, position.y + 20, 100, 25, bodyType::STATIC);
					pbodySword->ctype = ColliderType::SWORD;
					attackBodyCreated = true;
				}
				else if (currentAnim == &attack3Anim && currentAnim->GetCurrentFrameCount() >= 5 && !attackBodyCreated)
				{
					if (isFacingRight)
					{
						vel.x = 80;
						pbodySword = app->physics->CreateRectangleSensor(position.x + 150, position.y + 20, 25, 25, bodyType::STATIC);
					}
					else
					{
						vel.x = -80;
						pbodySword = app->physics->CreateRectangleSensor(position.x - 50, position.y + 20, 25, 25, bodyType::STATIC);
					}
			
					pbodySword->ctype = ColliderType::SWORD;
					attackBodyCreated = true;
				}

				if (currentAnim->HasFinished())
				{
					isAttacking = false;
					attackBodyCreated = false;

					if (pbodySword != NULL) destroyAttackBody = true;
				}
			}

			if (jumper)
			{
				isJumping = true;
				currentAnim->Reset();
				if (!isEquipped) currentAnim = &jumpAnim;
				else currentAnim = &armorJumpAnim;
				vel = b2Vec2(vel.x, -1.0f * dt);
				pbody->body->SetLinearVelocity(vel);
				jumper = false;
			}

			if (isJumping && currentAnim->HasFinished() || (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && !wallLeft && !wallRight))
			{
				currentAnim->ResetLoopCount();
				currentAnim->Reset();
				if (!isEquipped) currentAnim = &idleAnim;
				else currentAnim = &armorIdleAnim;
				isJumping = false;
			}

			if (position.y != previousY && !wallLeft && !wallRight)
			{
				ground = false;

				if (pbody->body->GetLinearVelocity().y > 0)
				{
					if (!isEquipped) currentAnim = &fallAnim;
					else currentAnim = &armorFallAnim;
				}
			}
			else if (!wallLeft && !wallRight)
			{
				ground = true;
			}

			if (activeSword && currentAnim != &swordAnim)
			{
				currentAnim = &swordAnim;
				currentAnim->ResetLoopCount();
				currentAnim->Reset();
			}

			if (enterDoor && currentAnim != &doorAnim)
			{
				currentAnim = &doorAnim;
				currentAnim->ResetLoopCount();
				currentAnim->Reset();
			}

			if (activeSword && currentAnim->HasFinished()) activeSword = false;
		
			previousY = position.y;

			if (!isJumping && !wallLeft && !wallRight && !isDashing) vel.y = -GRAVITY_Y;
			pbody->body->SetLinearVelocity(vel);
		}
		else
		{
			vel.SetZero();

			if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			{
				vel.y = -speed * 2 * dt;
			}
			if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			{
				vel.y = speed * 2 * dt;
			}
			if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			{
				vel.x = -speed * 2 * dt;
			}
			if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			{
				vel.x = speed * 2 * dt;
			}

			pbody->body->SetLinearVelocity(vel);
		}

		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 50;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 42;
	}
	else
	{
		pbody->body->SetLinearVelocity({0, 0});
		if (!isEquipped && currentAnim!= &deadAnim)
		{
			currentAnim = &deadAnim;
			currentAnim->ResetLoopCount();
			currentAnim->Reset();
		}
		else if (isEquipped && currentAnim != &armorDeadAnim)
		{
			currentAnim = &armorDeadAnim;
			currentAnim->ResetLoopCount();
			currentAnim->Reset();
		}
		if (!isEquipped && currentAnim == &deadAnim && currentAnim->HasFinished()) SetToInitialPosition();
		else if (isEquipped && currentAnim == &armorDeadAnim && currentAnim->HasFinished()) SetToInitialPosition();
	}

	if (app->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN) isEquipped = !isEquipped;

	app->render->DrawTexture(lightTexture, position.x-150, position.y-165, NULL, SDL_FLIP_NONE, 1.0f);
    SDL_Rect rect = currentAnim->GetCurrentFrame();

	int yOffset = 0;
	if (!isCrouching) yOffset = 0;
	else yOffset = 10;

	if (wallRight || wallLeft) currentEffectsAnim = &climbEffectAnim;
	if (isPushing && !canPush) currentEffectsAnim = &dangerEffectAnim;

	if (isFacingRight)
	{
		app->render->DrawTexture(texture, position.x, position.y - yOffset, &rect);
		if (wallRight || wallLeft || (isPushing && !canPush)) app->render->DrawTexture(effectsTexture, position.x + 12, position.y - 32, &currentEffectsAnim->GetCurrentFrame());
		else if (isJumping) app->render->DrawTexture(effectsTexture, position.x + 12, position.y + 32, &currentEffectsAnim->GetCurrentFrame());
	}
	else
	{
		app->render->DrawTexture(texture, position.x, position.y - yOffset, &rect, SDL_FLIP_HORIZONTAL);
		if (wallRight || wallLeft || (isPushing && !canPush)) app->render->DrawTexture(effectsTexture, position.x + 24, position.y - 32, &currentEffectsAnim->GetCurrentFrame(), SDL_FLIP_HORIZONTAL);
		else if (isJumping) app->render->DrawTexture(effectsTexture, position.x + 24, position.y + 32, &currentEffectsAnim->GetCurrentFrame(), SDL_FLIP_HORIZONTAL);
	}

    currentAnim->Update();
	if (wallRight || wallLeft || isJumping || (isPushing && !canPush)) currentEffectsAnim->Update();
	//printf("\r jumpcount: %d	ground: %d	isJumping: %d	hadJumped: %d	wall: %d", jumpCount, ground,isJumping,hasJumped,wall);
	/*printf("\r playerX: %d playerY: %d", position.x, position.y);*/
	printf("\r cameraX: %d cameraY: %d positionX: %d positionY %d", app->render->camera.x, app->render->camera.y, position.x, position.y);
    return true;
}

bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::KEY:
		LOG("Collision KEY");
		app->audio->PlayFx(pickCoinFxId);
		keys++;
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		ground = true;
		jumpCount = 0;
		dashCount = 0;
		hasJumped = false;
		isJumping = false;
		wallLeft = false;
		wallRight = false;
		wallEnd = false;
		if (!godMode) pbody->body->SetGravityScale(1.0f);
		break;
	case ColliderType::L_WALL:
		LOG("Collision L_WALL");
		wallLeft = true;
		break;
	case ColliderType::R_WALL:
		LOG("Collision R_WALL");
		wallRight = true;
		break;
	case ColliderType::SPIKE:
		LOG("Collision SPIKE");
		if(!godMode && !isDead) isDead = true;
		break;
	case ColliderType::JUMP:
		LOG("Collision JUMP");
		pbody->body->SetGravityScale(1.0f);
		jumper = true;
		break;
	case ColliderType::CAMERA:
		LOG("Collision CAMERA");
		if ((app->scene->cameraIdx == 0 && isFacingRight) 
			|| (app->scene->cameraIdx == 3 && position.x < 1400 && position.y > 5200)
			|| (app->scene->cameraIdx == 4 && position.x > 2000 && position.x < 2430 && position.y < 5200) 
			|| (app->scene->cameraIdx == 5 && position.x > 3200 && position.x < 3430 && position.y > 3515)
			|| (app->scene->cameraIdx == 6 && position.x > 3760 && position.x < 4000 && position.y > 2716)
			|| (app->scene->cameraIdx == 7 && position.x > 3800 && position.x < 4150 && position.y > 2000 && position.y < 2300)
			|| (app->scene->cameraIdx == 8 && position.x < 5030 && position.x > 4800))
		{
			app->scene->cameraIdx++;
			app->scene->cameraInitialized = true;
		}
		else if ((app->scene->cameraIdx == 1 && !isFacingRight)
			|| (app->scene->cameraIdx == 4 && position.x < 1400 && position.y < 5200)
			|| (app->scene->cameraIdx == 5 && position.x < 2000 && position.x < 2430 && position.y > 5200)
			|| (app->scene->cameraIdx == 6 && position.x > 3200 && position.x < 3430 && position.y < 3515)
			|| (app->scene->cameraIdx == 7 && position.x > 3760 && position.x < 4000 && position.y < 2716)
			|| (app->scene->cameraIdx == 8 && position.x > 3800 && position.x < 4150 && position.y < 2000)
			|| (app->scene->cameraIdx == 9 && position.x > 5070))
		{
			app->scene->cameraIdx--;
			app->scene->cameraInitialized = true;
		}
		break;
	case ColliderType::WALL_END:
		if (wallLeft || wallRight) wallEnd = true;
		else ground = true;
		break;
	case ColliderType::WALL_DOWN:
		wallEnd = false;
		break;
	case ColliderType::OBSTACLE:
		if (!isJumping) isPushing = true;
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}

}

void Player::SetToInitialPosition()
{
	if (app->scene->level1Enabled)
	{
		app->fade->Fade(1,60);
	}
	else if (app->scene->level2Enabled)
	{
		app->fade->Fade(2, 60);
	}
	else if (app->scene->level3Enabled)
	{
		app->fade->Fade(3, 60);
	}
}

void Player::LoadAnimations()
{
	idleAnim.LoadAnimations("idleAnim", "player");
	jumpAnim.LoadAnimations("jumpAnim", "player");
	walkAnim.LoadAnimations("walkAnim", "player");
	crouchAnim.LoadAnimations("crouchAnim", "player");
	crouchWalkAnim.LoadAnimations("crouchWalkAnim", "player");
	fallAnim.LoadAnimations("fallAnim", "player");
	wallAnim.LoadAnimations("wallAnim", "player");
	deadAnim.LoadAnimations("deadAnim", "player");

	armorIdleAnim.LoadAnimations("armorIdleAnim", "player");
	armorJumpAnim.LoadAnimations("armorJumpAnim", "player");
	armorWalkAnim.LoadAnimations("armorWalkAnim", "player");
	armorCrouchAnim.LoadAnimations("armorCrouchAnim", "player");
	armorCrouchWalkAnim.LoadAnimations("armorCrouchWalkAnim", "player");
	armorFallAnim.LoadAnimations("armorFallAnim", "player");
	armorDeadAnim.LoadAnimations("armorDeadAnim", "player");
	attack1Anim.LoadAnimations("attack1Anim", "player");
	attack2Anim.LoadAnimations("attack2Anim", "player");
	attack3Anim.LoadAnimations("attack3Anim", "player");
	attackJumpAnim.LoadAnimations("attackJumpAnim", "player");
	pushAnim.LoadAnimations("pushAnim", "player");
	swordAnim.LoadAnimations("swordAnim", "player");
	doorAnim.LoadAnimations("doorAnim", "player");

	climbEffectAnim.LoadAnimations("climbEffectAnim", "player");
	jumpEffectAnim.LoadAnimations("jumpEffectAnim", "player");
	dangerEffectAnim.LoadAnimations("dangerEffectAnim", "player");
}

void Player::ToggleGodMode()
{
	godMode = !godMode;

	if (godMode)
	{
		pbody->body->GetFixtureList()->SetSensor(true);
		pbody->body->SetGravityScale(0.0f);
		pbody->body->SetLinearVelocity({ 0, 0 });
		currentAnim = &idleAnim;
	}
	else
	{
		pbody->body->GetFixtureList()->SetSensor(false);
		pbody->body->SetGravityScale(1.0f);
	}

}