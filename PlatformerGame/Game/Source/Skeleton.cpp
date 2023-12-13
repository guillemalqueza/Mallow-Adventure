#include "Skeleton.h"
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
#include "Map.h"

Skeleton::Skeleton() : Entity(EntityType::SKELETON)
{
	name.Create("Skeleton");
}

Skeleton::~Skeleton() {}

bool Skeleton::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	speed = parameters.attribute("speed").as_float();

	return true;
}

bool Skeleton::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pathTexture = app->tex->Load("Assets/Textures/path.png");
	pbody = app->physics->CreateCircle(position.x, position.y, 20, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::SKELETON;
	pbody->listener = this;

	enemyPbody = app->physics->CreateRectangleSensor(position.x, position.y, 30, 54, bodyType::KINEMATIC);
	enemyPbody->ctype = ColliderType::ENEMY;
	enemyPbody->listener = this;

	initialTransform = pbody->body->GetTransform();

	LoadAnimations();

	return true;
}

bool Skeleton::Update(float dt)
{
	playerTilePos = app->map->WorldToMap(app->scene->player->position.x + 50, app->scene->player->position.y + 64);
	skeletonTilePos = app->map->WorldToMap(position.x, position.y);

	distance = sqrt(pow(playerTilePos.x - skeletonTilePos.x, 2) + pow(playerTilePos.y - skeletonTilePos.y, 2));

	if (distance < 10)
	{
		app->map->pathfinding->CreatePath(skeletonTilePos, playerTilePos);

		if (distance < 4)
		{
			currentAnim = &skeletonAttackAnim;
			velocity = { 0, 0 };
		}
		else if (distance >= 4)
		{
			currentAnim = &skeletonWalkAnim;
			Move(skeletonTilePos, playerTilePos);
		}
		else
		{
			currentAnim = &skeletonIdleAnim;
			velocity = { 0, 0 };
			app->map->pathfinding->ClearLastPath();
		}
	}

	if (health <= 0)
	{
		currentAnim = &skeletonDeadAnim;
		velocity = { 0, 0 };
		isDead = true;
	}

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

	pbody->body->SetLinearVelocity(velocity);
	enemyPbody->body->SetTransform({ pbody->body->GetPosition().x, pbody->body->GetPosition().y - PIXEL_TO_METERS(10) }, 0);

	SDL_Rect rect = currentAnim->GetCurrentFrame();
	if (isFacingRight) app->render->DrawTexture(texture, position.x - 60, position.y - 75, &rect);
	else app->render->DrawTexture(texture, position.x - 65, position.y - 75, &rect, SDL_FLIP_HORIZONTAL);

	currentAnim->Update();

	if (app->physics->debug)
	{
		const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			app->render->DrawTexture(pathTexture, pos.x, pos.y);
		}
	}

	return true;
}

bool Skeleton::CleanUp()
{
	return true;
}

void Skeleton::Move(const iPoint& origin, const iPoint& destination) {

	float xDiff = destination.x - origin.x;
	float yDiff = destination.y - origin.y;

	iPoint playerTilePos = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);
	if (app->map->pathfinding->IsWalkable(playerTilePos) != 0)
	{
		if (xDiff < 0)
		{
			velocity.x = -2;
			isFacingRight = false;
		}
		if (xDiff > 0)
		{
			velocity.x = 2;
			isFacingRight = true;
		}

		/*if (yDiff < 0)
		{
			velocity.y = -2;
		}
		if (yDiff > 0)
		{
			velocity.y = 2;
		}*/
	}
	else velocity = { 0,0 };
}

void Skeleton::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
		case ColliderType::SWORD:
			health -= 50;
			break;
	}
}

void Skeleton::LoadAnimations()
{
	skeletonIdleAnim.LoadAnimations("skeletonIdleAnim", "skeleton");
	skeletonAttackAnim.LoadAnimations("skeletonAttackAnim", "skeleton");
	skeletonDeadAnim.LoadAnimations("skeletonDeadAnim", "skeleton");
	skeletonWalkAnim.LoadAnimations("skeletonWalkAnim", "skeleton");
	skeletonHurtAnim.LoadAnimations("skeletonHurtAnim", "skeleton");

	currentAnim = &skeletonIdleAnim;
}