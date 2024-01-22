#include "Life.h"
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

Life::Life() : Entity(EntityType::LIFE)
{
	name.Create("life");
}

Life::~Life() {}

bool Life::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	lifeAudioFxId = app->audio->LoadFx(parameters.child("chestAudio").attribute("path").as_string());

	return true;
}

bool Life::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x + 32, position.y + 32, 20, bodyType::STATIC);
	pbody->ctype = ColliderType::CHEST;
	pbody->body->GetFixtureList()->SetSensor(true);
	pbody->listener = this;

	lifePotionAnim.LoadAnimations("lifePotionAnim", "life");
	currentAnim = &lifePotionAnim;

	return true;
}

bool Life::Update(float dt)
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	if (!isPicked) app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame());

	currentAnim->Update();
	return true;
}

bool Life::CleanUp()
{
	return true;
}

void Life::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physA == pbody) {
		switch (physB->ctype)
		{
		case ColliderType::PLAYER:
			LOG("Collision PLAYER");
			if (app->scene->player->health < 100)
			{
				pbody->body->SetActive(false);
				app->scene->player->isDrinkingLives = true;
				isPicked = true;
			}
		
			break;
		}
	}
}