#include "Chest.h"
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

Chest::Chest() : Entity(EntityType::CHEST)
{
	name.Create("chest");
}

Chest::~Chest() {}

bool Chest::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Chest::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangleSensor(position.x + 90, position.y + 190, 25, 200, bodyType::STATIC);
	pbody->ctype = ColliderType::CHEST;
	pbody->listener = this;

	pbody2 = app->physics->CreateCircle(position.x + 90, position.y + 190, 250, bodyType::STATIC);
	pbody2->body->GetFixtureList()->SetSensor(true);
	pbody2->ctype = ColliderType::EQUIPMENT_AREA;
	pbody2->listener = this;

	equipmentWithSword.LoadAnimations("equipmentWithSword", "equipment");
	equipmentAnim.LoadAnimations("equipmentAnim", "equipment");
	equipmentWithoutSword.LoadAnimations("equipmentWithoutSword", "equipment");
	equipmentAnimIdle.LoadAnimations("equipmentAnimIdle", "equipment");
	currentAnim = &equipmentWithSword;


	return true;
}

bool Chest::Update(float dt)
{
	if (isPicked)	OnPicked();
	if (equipmentArea) {
		currentAnim = &equipmentAnim;
		OnSensor();
	}
	if (currentAnim == &equipmentAnim && currentAnim->HasFinished())
	{
		currentAnim = &equipmentAnimIdle;
	}


	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 90;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 190;

	app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame());

	currentAnim->Update();
	return true;
}

bool Chest::CleanUp()
{
	return true;
}

void Chest::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physA == pbody) {
		switch (physB->ctype)
		{
		case ColliderType::PLAYER:
			LOG("Collision PLAYER");
			if (!isPicked)
			{
				pbody->body->SetActive(false);
				app->scene->player->activeSword = true;
				app->scene->player->isEquipped = true;
				isPicked = true;
			}
			break;
		}
	}
	else if (physA == pbody2) {
		switch (physB->ctype)
		{
		case ColliderType::PLAYER:
			LOG("Collision PLAYER");
			if (!equipmentArea)
			{
				pbody2->body->SetActive(false);
				equipmentArea = true;
			}
			break;
		}
	}
}

void Chest::OnPicked()
{
	currentAnim = &equipmentWithoutSword;
	//app->entityManager->DestroyEntity(this);
	//app->physics->world->DestroyBody(pbody->body);
	isPicked = false;
}

void Chest::OnSensor()
{
	app->physics->world->DestroyBody(pbody2->body);
	equipmentArea = false;
}