#include "Key.h"
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

Key::Key() : Entity(EntityType::KEY)
{
	name.Create("key");
}

Key::~Key() {}

bool Key::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Key::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::STATIC);
	pbody->ctype = ColliderType::KEY;
	pbody->listener = this;

	keyIdleAnim.LoadAnimations("keyIdleAnim", "key");

	followTimer = 0.0f;
	currentAnim = &keyIdleAnim;
	return true;
}

bool Key::Update(float dt)
{
	if (isPicked) OnPicked();

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame());

	currentAnim->Update();
	return true;
}

bool Key::CleanUp()
{
	return true;
}

void Key::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		pbody->body->SetActive(false);
		isPicked = true;
		break;
	}
}

void Key::OnPicked()
{
	followTimer += 0.1f;

	iPoint playerPosition = app->scene->player->position;
	float distanceX = abs(playerPosition.x - position.x);
	if (followTimer > 10.0f && distanceX <= 10.0f)
	{
		app->entityManager->DestroyEntity(this);
		app->physics->world->DestroyBody(pbody->body);
	}
	else
	{
		acceleration += 0.2f;
		float bodyX = position.x + (playerPosition.x - position.x) * 0.01f * acceleration;
        float bodyY = position.y + (playerPosition.y - position.y) * 0.01f * acceleration;
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(bodyX + 0.32f), PIXEL_TO_METERS(bodyY + 0.32f)), 0);
	}
}