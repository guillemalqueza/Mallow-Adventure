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

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	idleAnim.PushBack({ 0,1216,100,64 });
	idleAnim.PushBack({ 100,1216,100,64 });
	idleAnim.PushBack({ 200,1216,100,64 });
	idleAnim.PushBack({ 300,1216,100,64 });
	idleAnim.loop = true;
	idleAnim.speed = 0.1f;

	jumpAnim.PushBack({ 0,1344,100,64 });
	jumpAnim.PushBack({ 100,1344,100,64 });
	jumpAnim.PushBack({ 200,1344,100,64 });
	jumpAnim.PushBack({ 300,1344,100,64 });
	jumpAnim.PushBack({ 400,1344,100,64 });
	jumpAnim.PushBack({ 500,1344,100,64 });
	jumpAnim.PushBack({ 0,1344,100,64 });
	jumpAnim.loop = false;
	jumpAnim.speed = 0.2f;

	walkAnim.PushBack({ 0,2176,100,64 });
	walkAnim.PushBack({ 100,2176,100,64 });
	walkAnim.PushBack({ 200,2176,100,64 });
	walkAnim.PushBack({ 300,2176,100,64 });
	walkAnim.PushBack({ 400,2176,100,64 });
	walkAnim.PushBack({ 500,2176,100,64 });
	walkAnim.PushBack({ 600,2176,100,64 });
	walkAnim.loop = true;
	walkAnim.speed = speed;

	crouchAnim.PushBack({ 0,256,100,64 });
	crouchAnim.PushBack({ 100,256,100,64 });
	crouchAnim.PushBack({ 200,256,100,64 });
	crouchAnim.loop = true;
	crouchAnim.speed = 0.1f;

	crouchWalkAnim.PushBack({ 0,512,100,64 });
	crouchWalkAnim.PushBack({ 100,512,100,64 });
	crouchWalkAnim.PushBack({ 200,512,100,64 });
	crouchWalkAnim.PushBack({ 300,512,100,64 });
	crouchWalkAnim.loop = true;
	crouchWalkAnim.speed = 0.1f;

	fallAnim.PushBack({ 0,896,100,64 });
	fallAnim.PushBack({ 100,896,100,64 });
	fallAnim.PushBack({ 200,896,100,64 });
	fallAnim.loop = true;
	fallAnim.speed = 0.1f;

	wallAnim.PushBack({ 0,1024,100,64 });
	wallAnim.PushBack({ 100,1024,100,64 });
	wallAnim.PushBack({ 200,1024,100,64 });
	wallAnim.loop = true;
	wallAnim.speed = 0.1f;


	deadAnim.PushBack({ 0,768,100,64 });
	deadAnim.PushBack({ 100,768,100,64 });
	deadAnim.PushBack({ 200,768,100,64 });
	deadAnim.PushBack({ 300,768,100,64 });
	deadAnim.PushBack({ 400,768,100,64 });
	deadAnim.loop = false;
	deadAnim.speed = 0.1f;

	currentAnim = &idleAnim;

	jumper = false;

	pbody = app->physics->CreateCircle(position.x + 50, position.y, 32, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
	if (!isDead)
	{
		if (!isJumping) currentAnim = &idleAnim;

		b2Vec2 vel = pbody->body->GetLinearVelocity();

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

		if (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
		{
			isCrouching = true;
			if (!isWalking) currentAnim = &crouchAnim;
		}
		else isCrouching = false;

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
			}
			isFacingRight = true;
		}

		if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_IDLE && !isDashing)
		{
			isWalking = false;
			vel.x = 0;
		}

		if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
			isRunning = true;
			speed = 0.5f;
		}
		else
		{
			isRunning = false;
			speed = 0.2f;
		}

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			if (!isJumping && !hasJumped && ground && jumpCount == 0)
			{
				currentAnim->Reset();
				currentAnim = &jumpAnim;
				isJumping = true;
				hasJumped = true;
				vel = b2Vec2(vel.x, -10.0f);
				pbody->body->SetLinearVelocity(vel);
				jumpCount = 1;
			}
			else if (jumpCount == 1 && hasJumped)
			{
				currentAnim->Reset();
				currentAnim = &jumpAnim;
				isJumping = true;
				vel = b2Vec2(vel.x, -10.0f);
				pbody->body->SetLinearVelocity(vel);
				jumpCount = 2;

			}
			else if (!ground && !isJumping && !hasJumped && jumpCount == 0)
			{
				currentAnim->Reset();
				currentAnim = &jumpAnim;
				isJumping = true;
				vel = b2Vec2(vel.x, -10.0f);
				pbody->body->SetLinearVelocity(vel);
				jumpCount = 2;
			}

		}

		if (app->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN && !isDashing && !ground && dashCount == 0) {
			isDashing = true;
			dashTimer = 0.0f;

			if (isFacingUp && !isWalking)
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
				/*	printf("Falling");*/
			}
		}
		//else if (position.y > previousY && wall)
		//{
		//	currentAnim = &fallAnim;
		//}
		else if (!wall)
		{
			ground = true;
			//jumpCount = 0;
		}

		previousY = position.y;

		if (!isJumping && !wall && !isDashing) vel.y = -GRAVITY_Y;
		pbody->body->SetLinearVelocity(vel);

		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 50;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 32;
	}
	else
	{
		pbody->body->SetLinearVelocity({0, 0});
		if (currentAnim!= &deadAnim)
		{
			currentAnim->Reset();
		}
		currentAnim = &deadAnim;
		if (currentAnim->HasFinished())
		{
			pbody->body->SetTransform({ PIXEL_TO_METERS(447), PIXEL_TO_METERS(991)}, 0);
			isDead = false;
		}
	}

    SDL_Rect rect = currentAnim->GetCurrentFrame();
    if(isFacingRight) app->render->DrawTexture(texture, position.x, position.y, &rect);
	else app->render->DrawTexture(texture, position.x, position.y, &rect, SDL_FLIP_HORIZONTAL);
    currentAnim->Update();
	//printf("\r jumpcount: %d	ground: %d	isJumping: %d	hadJumped: %d	wall: %d", jumpCount, ground,isJumping,hasJumped,wall);
	printf("\r playerX: %d playerY: %d", position.x, position.y );
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
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		ground = true;
		jumpCount = 0;
		dashCount = 0;
		hasJumped = false;
		isJumping = false;
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
		isDead = true;
		break;
	case ColliderType::JUMP:
		LOG("Collision JUMP");
		jumper = true;
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}

}