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
	initialIdlePosition = position.x;

	LoadAnimations();

	return true;
}

bool Skeleton::Update(float dt)
{
	playerTilePos = app->map->WorldToMap(app->scene->player->position.x + 50, app->scene->player->position.y + 64);
	skeletonTilePos = app->map->WorldToMap(position.x, position.y);

	distance = playerTilePos.DistanceTo(skeletonTilePos);

	if (destroyAttackBody)
	{
		pbodySword->body->GetWorld()->DestroyBody(pbodySword->body);
		pbodySword = NULL;
		destroyAttackBody = false;
	}

	if (distance < 10)
	{
		app->map->pathfinding->CreatePath(skeletonTilePos, playerTilePos);

		if (distance < 3 && !isAttacking)
		{
			isAttacking = true;
			currentAnim = &skeletonAttackAnim;
			currentAnim->ResetLoopCount();
			currentAnim->Reset();
			velocity = { 0, -GRAVITY_Y};

		}
		else if (distance >= 3 && !isAttacking)
		{
			const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
			if (path->Count() > 1) {
				nextTilePath = { path->At(1)->x, path->At(1)->y};
				Move(skeletonTilePos, nextTilePath);
			}
			currentAnim = &skeletonWalkAnim;
		}
		else if (!isAttacking)
		{
			currentAnim = &skeletonIdleAnim;
			velocity = { 0, -GRAVITY_Y };
			app->map->pathfinding->ClearLastPath();
		}
	}
	else
	{
		const int idleDistance = 3;

		if (position.x >= initialIdlePosition + idleDistance * 32)
		{
			isFacingRight = false;
		}
		else if (position.x <= initialIdlePosition - idleDistance * 32)
		{
			isFacingRight = true;
		}

		velocity.x = isFacingRight ? 1 : -1;

		position.x += velocity.x;

		currentAnim = &skeletonWalkAnim;

	}

	if (isAttacking)
	{
		if (currentAnim == &skeletonAttackAnim && currentAnim->GetCurrentFrameCount() >= 4 && !attackBodyCreated)
		{
			if (isFacingRight) pbodySword = app->physics->CreateRectangleSensor(position.x + 50, position.y - 20, 30, 30, bodyType::STATIC);
			else pbodySword = app->physics->CreateRectangleSensor(position.x - 50, position.y - 20, 30, 30, bodyType::STATIC);
			pbodySword->ctype = ColliderType::ENEMY_SWORD;
			attackBodyCreated = true;
		}

		if (currentAnim == &skeletonAttackAnim && currentAnim->GetCurrentFrameCount() >= 9 && attackBodyCreated)
		{
			isAttacking = false;
			attackBodyCreated = false;

			if (pbodySword != NULL) destroyAttackBody = true;
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

void Skeleton::Move(const iPoint& origin, const iPoint& destination) 
{
	float xDiff = destination.x - origin.x;
	float yDiff = destination.y - origin.y;

	if (app->map->pathfinding->IsWalkable(destination) != 0)
	{
		velocity.x = (xDiff < 0) ? -2 : (xDiff > 0) ? 2 : 0;
		velocity.y = (yDiff < 0) ? -2 : (yDiff > 0) ? -GRAVITY_Y : 0;

		isFacingRight = (xDiff > 0);
	}
	else {
		velocity = { 0, -GRAVITY_Y };
	}
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