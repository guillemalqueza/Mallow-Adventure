#include "Equipment.h"
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

Equipment::Equipment() : Entity(EntityType::EQUIPMENT)
{
	name.Create("equipment");
}

Equipment::~Equipment() {}

bool Equipment::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Equipment::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::STATIC);
	pbody->ctype = ColliderType::EQUIPMENT;
	pbody->listener = this;

	equipmentIdleAnim.LoadAnimations("equipmentIdleAnim", "equipment");
	currentAnim = &equipmentIdleAnim;

	return true;
}

bool Equipment::Update(float dt)
{
	if (isPicked) OnPicked();

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame());

	currentAnim->Update();
	return true;
}

bool Equipment::CleanUp()
{
	return true;
}

void Equipment::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		if (!isPicked)
		{
			pbody->body->SetActive(false);
			app->scene->player->isEquipped = true;
			isPicked = true;
		}
		break;
	}
}

void Equipment::OnPicked()
{
	app->entityManager->DestroyEntity(this);
	app->physics->world->DestroyBody(pbody->body);
	isPicked = false;
}
