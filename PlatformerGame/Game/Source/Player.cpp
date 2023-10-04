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
Uint32 lastKeyPressTime = 0;

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

	currentAnim = &idleAnim;

	pbody = app->physics->CreateCircle(position.x + 50, position.y, 32, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	pickCoinFxId = app->audio->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
    if (!isJumping) currentAnim = &idleAnim;

    b2Vec2 vel = b2Vec2(0, -GRAVITY_Y);

    if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
    {
		if (wall)
		{
			vel = b2Vec2(0, -speed * dt);
			currentAnim = &wallAnim;
		}
    }
    if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
    {
		if (wall)
		{
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

    if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
    {
        vel = b2Vec2(-speed * dt, -GRAVITY_Y);
		if (!isJumping)
		{
			if (isCrouching) currentAnim = &crouchWalkAnim;
			else currentAnim = &walkAnim;
		}
		isWalking = true;
		isFacingRight = false;
    }

    if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
    {
        vel = b2Vec2(speed * dt, -GRAVITY_Y);
		if (!isJumping)
		{
			if (isCrouching) currentAnim = &crouchWalkAnim;
			else currentAnim = &walkAnim;
		}
		isWalking = true;
		isFacingRight = true;
    }

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE)
	{
		isWalking = false;
		isDashing = false;
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
		if (!isJumping && !hasJumped)
		{
			currentAnim->Reset();
			currentAnim = &jumpAnim;
			isJumping = true;
			pbody->body->ApplyLinearImpulse({ 0, -10.0f }, pbody->body->GetWorldCenter(), true);
			jumpCount = 1;
		}
		else if (jumpCount == 1)
		{
			pbody->body->ApplyLinearImpulse({ 0, -10.0f }, pbody->body->GetWorldCenter(), true);
			currentAnim->Reset();
			currentAnim = &jumpAnim;
			jumpCount = 0;
			hasJumped = true;
			
		}
		if (!ground && isJumping)
		{
			currentAnim->Reset();
			currentAnim = &jumpAnim;
			isJumping = true;
			pbody->body->ApplyLinearImpulse({ 0, -10.0f }, pbody->body->GetWorldCenter(), true);
			jumpCount = 0;
		}
    }	
	
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN) {

		Uint32 currentTime = SDL_GetTicks();
		if ((currentTime - lastKeyPressTime) <= 15 * dt)
		{
			//DASHEAR
			//printf("DASH \n");
			isDashing = true;
			pbody->body->ApplyLinearImpulse({ -20.0f, 0 }, pbody->body->GetWorldCenter(), true);
		}
		lastKeyPressTime = currentTime;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) {
		Uint32 currentTime = SDL_GetTicks();
		if ((currentTime - lastKeyPressTime) <= 15 * dt)
		{
			//DASHEAR;
			isDashing = true;
			pbody->body->ApplyLinearImpulse({ 20.0f, 0 }, pbody->body->GetWorldCenter(), true);
		}
		lastKeyPressTime = currentTime;
	}

	

    if (isJumping && currentAnim->HasFinished() || app->input->GetKey(SDL_SCANCODE_S)== KEY_REPEAT)
    {
        currentAnim->ResetLoopCount();
        currentAnim->Reset();
        currentAnim = &idleAnim;
        isJumping = false;
    }

	if (position.y != previousY && !isJumping && !wall && !isDashing)
	{
		ground = false;
		currentAnim = &fallAnim;
	}
	else
	{
		ground = true;
		//jumpCount = 0;
	}

	previousY = position.y;

	if(!ground) currentAnim = &fallAnim;

	if (!isJumping) pbody->body->SetLinearVelocity(vel);
	else if(!isDashing)
	{
		pbody->body->SetLinearVelocityX(vel.x);
	}

    position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 50;
    position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 32;

    SDL_Rect rect = currentAnim->GetCurrentFrame();
    if(isFacingRight) app->render->DrawTexture(texture, position.x, position.y, &rect);
	else app->render->DrawTexture(texture, position.x, position.y, &rect, SDL_FLIP_HORIZONTAL);
    currentAnim->Update();
	printf("\r jumpcount: %d ground=%d, isdashing=%d", jumpCount, ground,isDashing);
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
	case ColliderType::WALL:
		LOG("Collision WALL");
		wall = true;
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		ground = true;
		wall = false;
		jumpCount = 0;
		hasJumped = false;
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}

}