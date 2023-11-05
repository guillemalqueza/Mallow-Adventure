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
	speed = parameters.attribute("speed").as_float();
	pickCoinFxId = app->audio->LoadFx(parameters.child("itemAudio").attribute("path").as_string());
	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	lightTexture = app->tex->Load(lightTexturePath);
	LoadAnimations();

	currentAnim = &idleAnim;

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
		if (!isJumping) currentAnim = &idleAnim;

		b2Vec2 vel = pbody->body->GetLinearVelocity();

		if (!godMode)
		{
			//idle wall keys
			if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_IDLE && wall && !ground)
			{
				pbody->body->SetGravityScale(0.0f);
				pbody->body->SetLinearVelocity({ pbody->body->GetLinearVelocity().x, 0 });
				vel = b2Vec2(0, 0);
				currentAnim = &wallAnim;
			}
			else if (!wall)
			{
				pbody->body->SetGravityScale(1.0f);
			}

			if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_IDLE) isFacingUp = false;

			//climb wall keys
			if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			{
				isFacingUp = true;

				if (wall)
				{
					ground = false;
					vel = b2Vec2(0, -speed * dt);
					currentAnim = &wallAnim;
				}
			}
			if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			{
				if (wall)
				{
					ground = false;
					vel = b2Vec2(0, speed * dt);
					currentAnim = &wallAnim;
				}
			}

			//crouch
			if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
			{
				isCrouching = true;
				if (!isWalking) currentAnim = &crouchAnim;
			}
			else isCrouching = false;

			//player movement
			if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			{
				if (!isDashing) vel.x = -speed * dt;
				if (!isJumping && !wall)
				{
					if (isCrouching) currentAnim = &crouchWalkAnim;
					else currentAnim = &walkAnim;
				}
				isWalking = true;
				if (currentAnim == &wallAnim && !ground && isFacingRight)
				{
					wall = false;
					currentAnim = &idleAnim;
					pbody->body->SetGravityScale(1.0f);
				}
				isFacingRight = false;
			}

			if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			{
				if (!isDashing) vel.x = speed * dt;
				if (!isJumping && !wall)
				{
					if (isCrouching) currentAnim = &crouchWalkAnim;
					else currentAnim = &walkAnim;
				}
				isWalking = true;
				if (currentAnim == &wallAnim && !ground && !isFacingRight)
				{
					wall = false;
					currentAnim = &idleAnim;
					pbody->body->SetGravityScale(1.0f);
				}
				isFacingRight = true;
			}

			if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_IDLE && !isDashing)
			{
				isWalking = false;
				vel.x = 0;
			}

			//jump
			if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
			{
				if (!isJumping && !hasJumped && ground && jumpCount == 0)
				{
					currentAnim->Reset();
					currentAnim = &jumpAnim;
					isJumping = true;
					hasJumped = true;
					vel = b2Vec2(vel.x, -8.2f);
					pbody->body->SetLinearVelocity(vel);
					jumpCount = 1;
				}
				else if (jumpCount == 1 && hasJumped && dashCount == 0)
				{
					currentAnim->Reset();
					currentAnim = &jumpAnim;
					isJumping = true;
					vel = b2Vec2(vel.x, -8.2f);
					pbody->body->SetLinearVelocity(vel);
					jumpCount = 2;
					dashCount = 1;
				}
				else if (!ground && !isJumping && !hasJumped && jumpCount == 0 && dashCount == 0)
				{
					currentAnim->Reset();
					currentAnim = &jumpAnim;
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

			if (jumper)
			{
				isJumping = true;
				currentAnim->Reset();
				currentAnim = &jumpAnim;
				vel = b2Vec2(vel.x, -1.0f * dt);
				pbody->body->SetLinearVelocity(vel);
				jumper = false;
			}

			if (isJumping && currentAnim->HasFinished() || (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && !wall))
			{
				currentAnim->ResetLoopCount();
				currentAnim->Reset();
				currentAnim = &idleAnim;
				isJumping = false;
			}

			if (position.y != previousY && !wall)
			{
				ground = false;

				if (pbody->body->GetLinearVelocity().y > 0)
				{
					currentAnim = &fallAnim;
				}
			}
			else if (!wall)
			{
				ground = true;
			}

			previousY = position.y;

			if (!isJumping && !wall && !isDashing) vel.y = -GRAVITY_Y;
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
		if (currentAnim!= &deadAnim)
		{
			currentAnim = &deadAnim;
			currentAnim->ResetLoopCount();
			currentAnim->Reset();
		}
		if (currentAnim == &deadAnim && currentAnim->HasFinished()) SetToInitialPosition();
	}

	app->render->DrawTexture(lightTexture, position.x-150, position.y-165, NULL, SDL_FLIP_NONE, 1.0f);
    SDL_Rect rect = currentAnim->GetCurrentFrame();
    if(isFacingRight) app->render->DrawTexture(texture, position.x, position.y, &rect);
	else app->render->DrawTexture(texture, position.x, position.y, &rect, SDL_FLIP_HORIZONTAL);
    currentAnim->Update();
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
	case ColliderType::ITEM:
		LOG("Collision ITEM");
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
		wall = false;
		if (!godMode) pbody->body->SetGravityScale(1.0f);
		break;
	case ColliderType::L_WALL:
		LOG("Collision L_WALL");
		wall = true;
		break;
	case ColliderType::R_WALL:
		LOG("Collision R_WALL");
		wall = true;
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
		if (app->scene->cameraIdx == 0 && isFacingRight)
		{
			app->scene->cameraIdx++;
			app->scene->cameraInitialized = true;
		}
		else if (app->scene->cameraIdx == 1 && !isFacingRight)
		{
			app->scene->cameraIdx--;
			app->scene->cameraInitialized = true;
		}
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}

}

void Player::SetToInitialPosition()
{
	if (!app->scene->level2Enabled)
	{
		app->fade->Fade(1,60);
	}
	else
	{
		app->fade->Fade(2, 60);
	}
}

void Player::LoadAnimations()
{
	idleAnim.LoadAnimations("idleAnim");
	jumpAnim.LoadAnimations("jumpAnim");
	walkAnim.LoadAnimations("walkAnim");
	crouchAnim.LoadAnimations("crouchAnim");
	crouchWalkAnim.LoadAnimations("crouchWalkAnim");
	fallAnim.LoadAnimations("fallAnim");
	wallAnim.LoadAnimations("wallAnim");
	deadAnim.LoadAnimations("deadAnim");
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