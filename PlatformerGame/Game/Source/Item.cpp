#include "Item.h"
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

Item::Item() : Entity(EntityType::ITEM)
{
	name.Create("item");
}

Item::~Item() {}

bool Item::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Item::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::STATIC);
	pbody->ctype = ColliderType::ITEM;
	pbody->listener = this;

	idleAnim.PushBack({ 0,0,32,32 });
	idleAnim.PushBack({ 32,0,32,32 });
	idleAnim.PushBack({ 64,0,32,32 });
	idleAnim.PushBack({ 96,0,32,32 });
	idleAnim.PushBack({ 128,0,32,32 });
	idleAnim.PushBack({ 160,0,32,32 });
	idleAnim.PushBack({ 192,0,32,32 });
	idleAnim.PushBack({ 224,0,32,32 });
	idleAnim.PushBack({ 256,0,32,32 });
	idleAnim.PushBack({ 288,0,32,32 });
	idleAnim.PushBack({ 320,0,32,32 });
	idleAnim.PushBack({ 352,0,32,32 });
	idleAnim.PushBack({ 384,0,32,32 });
	idleAnim.PushBack({ 416,0,32,32 });
	idleAnim.loop = true;
	idleAnim.speed = 0.1f;
	followTimer = 0.0f;
	currentAnim = &idleAnim;
	return true;
}

bool Item::Update(float dt)
{
	currentAnim = &idleAnim;

	if (isPicked)
	{
		followTimer += 0.1f;

		if (followTimer < 10.0f)
		{
			iPoint playerPosition = app->scene->player->position;

			position.x = static_cast<int>(position.x + (playerPosition.x - position.x) * 0.1f);
			position.y = static_cast<int>(position.y + (playerPosition.y - position.y) * 0.1f);
			app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame());
		}
		else
		{
			app->entityManager->DestroyEntity(this);
		}
	}
	else
	{
		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

		app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame());
	}

	currentAnim->Update();
	return true;
}

bool Item::CleanUp()
{
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		pbody->body->SetActive(false);
		app->physics->world->DestroyBody(pbody->body);
		isPicked = true;
		break;
	}
}