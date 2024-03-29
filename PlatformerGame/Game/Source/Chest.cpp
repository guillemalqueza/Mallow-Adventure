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

	/*position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();*/

	return true;
}

bool Chest::Start() {

	texturePath = parameters.attribute("texturepath").as_string();
	chestAudioFxId = app->audio->LoadFx(parameters.child("chestAudio").attribute("path").as_string());
	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangleSensor(position.x + 38, position.y + 34, 25, 200, bodyType::STATIC);
	pbody->ctype = ColliderType::CHEST;
	pbody->listener = this;

	pbody2 = app->physics->CreateCircle(position.x + 38, position.y + 38, 250, bodyType::STATIC);
	pbody2->body->GetFixtureList()->SetSensor(true);
	pbody2->ctype = ColliderType::CHEST_AREA;
	pbody2->listener = this;

	chestClosedIdleAnim.LoadAnimations("chestClosedIdleAnim", "chest");
	chestOpenAnim.LoadAnimations("chestOpenAnim", "chest");
	chestOpenPotionAnim.LoadAnimations("chestOpenPotionAnim", "chest");
	chestOpenedIdleAnim.LoadAnimations("chestOpenedIdleAnim", "chest");
	currentAnim = &chestClosedIdleAnim;


	return true;
}

bool Chest::Update(float dt)
{
	if (isPicked)	OnPicked();
	if (chestArea) {
		currentAnim = &chestOpenAnim;
		app->audio->PlayFx(chestAudioFxId);
		OnSensor();
	}
	if (currentAnim == &chestOpenAnim && currentAnim->HasFinished())
	{
		currentAnim = &chestOpenPotionAnim;
	}


	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 38;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 38;

	app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame());

	currentAnim->Update();
	return true;
}

bool Chest::CleanUp()
{
	app->tex->UnLoad(texture);
	return true;
}

void Chest::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physA == pbody) {
		switch (physB->ctype)
		{
		case ColliderType::PLAYER:
			LOG("Collision PLAYER");
			if (!isPicked && app->scene->player->canOpen)
			{
				pbody->body->SetActive(false);
				app->scene->player->canPush = true;
				app->scene->player->isDrinkingLives = true;
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
			if (!chestArea && app->scene->player->canOpen)
			{
				pbody2->body->SetActive(false);
				chestArea = true;
			}
			break;
		}
	}
}

void Chest::OnPicked()
{
	currentAnim = &chestOpenedIdleAnim;
	isPicked = false;
}

void Chest::OnSensor()
{
	app->physics->world->DestroyBody(pbody2->body);
	chestArea = false;
}