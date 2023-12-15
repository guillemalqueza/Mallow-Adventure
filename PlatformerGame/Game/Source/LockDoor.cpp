#include "LockDoor.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "EntityManager.h"
#include "FadeToBlack.h"

LockDoor::LockDoor() : Entity(EntityType::LOCK_DOOR)
{
	name.Create("LockDoor");
}

LockDoor::~LockDoor() {}

bool LockDoor::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool LockDoor::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangleSensor(position.x, position.y, 64, 64, bodyType::STATIC);
	pbody->ctype = ColliderType::DOOR;
	pbody->listener = this;
	
	doorIdleAnim.LoadAnimations("doorIdleAnim", "door");
	doorOpenAnim.LoadAnimations("doorOpenAnim", "door");

	currentAnim = &doorIdleAnim;

	return true;
}

bool LockDoor::Update(float dt)
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 192/2;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 128/2;

	if (isActivated)
	{
		isActivated = false;
		currentAnim = &doorOpenAnim;
	}

	if (currentAnim == &doorOpenAnim && currentAnim->HasFinished())
	{
		app->fade->Fade(2, 60);
		app->physics->world->DestroyBody(pbody->body);
		app->entityManager->DestroyEntity(this);
	}

	SDL_Rect rect = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect);
	currentAnim->Update();

	return true;
}

bool LockDoor::CleanUp()
{
	return true;
}

void LockDoor::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		if (app->scene->player->keys > 0)
		{
			isActivated = true;
			app->scene->player->keys--;
			app->scene->player->enterDoor = true;
		}
		break;
	}
}