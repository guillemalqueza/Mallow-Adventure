#include "Skeleton.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
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
	open = parameters.attribute("open").as_bool();
	attackAudio1FxId = app->audio->LoadFx(parameters.child("attackAudio1").attribute("path").as_string());
	attackAudio2FxId = app->audio->LoadFx(parameters.child("attackAudio2").attribute("path").as_string());
	attackAudio3FxId = app->audio->LoadFx(parameters.child("attackAudio3").attribute("path").as_string());
	hurtAudioFxId = app->audio->LoadFx(parameters.child("hittedAudio1").attribute("path").as_string());
	deathAudioFxId = app->audio->LoadFx(parameters.child("deadAudio1").attribute("path").as_string());
		
	return true;
}

bool Skeleton::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pathTexture = app->tex->Load("Assets/Textures/path.png");
	pbody = app->physics->CreateCircle(position.x, position.y, 20, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::SKELETON;
	pbody->listener = this;

	enemyPbody = app->physics->CreateRectangleSensor(position.x, position.y, 30, 54, bodyType::DYNAMIC);
	enemyPbody->ctype = ColliderType::ENEMY;
	enemyPbody->listener = this;

	initialTransform = pbody->body->GetTransform();
	initialIdlePosition = position.x;

	LoadAnimations();

	return true;
}

bool Skeleton::Update(float dt)
{
	if (!isDead)
	{
		playerTilePos = app->map->WorldToMap(app->scene->player->position.x + 50, app->scene->player->position.y + 64);
		skeletonTilePos = app->map->WorldToMap(position.x, position.y);

		distance = playerTilePos.DistanceTo(skeletonTilePos);

		if (destroyAttackBody)
		{
			if (pbodySword != NULL) pbodySword->body->GetWorld()->DestroyBody(pbodySword->body);
			pbodySword = NULL;
			destroyAttackBody = false;
		}

		if (distance < 10)
		{
			app->map->pathfinding->CreatePath(skeletonTilePos, playerTilePos);
			path = app->map->pathfinding->GetLastPath();

			if (distance < 3 && !isAttacking)
			{
				isAttacking = true;
				hasAttacked = true;
				currentAnim = &skeletonAttackAnim;
				int random = rand() % 3;
				switch (random){
				case 0:
					app->audio->PlayFx(attackAudio1FxId);
					break;
				case 1:
					app->audio->PlayFx(attackAudio2FxId);
					break;
				case 2:
					app->audio->PlayFx(attackAudio3FxId);
					break;
				}
				currentAnim->ResetLoopCount();
				currentAnim->Reset();
				velocity = { 0, -GRAVITY_Y };

			}
			else if (distance >= 3 && !isAttacking)
			{
				
				if (path->Count() > 1) {
					nextTilePath = { path->At(1)->x, path->At(1)->y };
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

			if (path == app->map->pathfinding->GetLastPath()) app->map->pathfinding->ClearLastPath();

		}

		if (isAttacking)
		{
			if (currentAnim == &skeletonAttackAnim && currentAnim->GetCurrentFrameCount() >= 4 && !attackBodyCreated)
			{
				if (isFacingRight) pbodySword = app->physics->CreateRectangleSensor(position.x + 50, position.y - 20, 30, 30, bodyType::STATIC);
				else pbodySword = app->physics->CreateRectangleSensor(position.x - 50, position.y - 20, 30, 30, bodyType::STATIC);
				pbodySword->ctype = ColliderType::ENEMY_SWORD;
				pbodySword->listener = this;
				attackBodyCreated = true;
			}

			if (currentAnim == &skeletonAttackAnim && currentAnim->GetCurrentFrameCount() >= 9 && attackBodyCreated)
			{
				isAttacking = false;
				hasAttacked = false;
				attackBodyCreated = false;

				if (pbodySword != NULL) destroyAttackBody = true;
			}
		}
	}

	if (health <= 0 && !isDead)
	{
		currentAnim = &skeletonDeadAnim;
		velocity = { 0, 0 };
		isDead = true;
		app->audio->PlayFx(deathAudioFxId);
		if (isAttacking)
		{
			isAttacking = false;
			destroyAttackBody = true;
		}
		pbody->body->SetActive(false);
		enemyPbody->body->SetActive(false);
		if (path == app->map->pathfinding->GetLastPath()) app->map->pathfinding->ClearLastPath();
	}

	if (isDead && open)
	{
		app->scene->player->canOpen = true;
		open = false;
	}

	if (setLoadPosition)
	{
		pbody->body->SetTransform({ PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y) }, 0);
		setLoadPosition = false;
	}
	else
	{ 
		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);
	}

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

	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		ResetEntity();
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
	if (physA->ctype == ColliderType::ENEMY )
	{
		switch (physB->ctype)
		{
			case ColliderType::SWORD:
				health -= 50;
				break;
		}
	}

	if (physA->ctype == ColliderType::ENEMY_SWORD)
	{
		switch (physB->ctype)
		{
			case ColliderType::PLAYER_BODY:
				if (app->scene->player->health > 0 && !hasAttacked) app->scene->player->health -= 1;
				break;
		}
	}
}

void Skeleton::ResetEntity()
{
	health = 100;
	isDead = false;
	pbody->body->SetActive(true);
	enemyPbody->body->SetActive(true);
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