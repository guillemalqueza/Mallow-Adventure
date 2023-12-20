#include "Ghost.h"
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

Ghost::Ghost() : Entity(EntityType::GHOST)
{
	name.Create("Ghost");
}

Ghost::~Ghost() {}

bool Ghost::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	speed = parameters.attribute("speed").as_float();
	summonPosition.x = parameters.attribute("x").as_int() - 100;
	summonPosition.y = parameters.attribute("y").as_int() - 100;
	ghostSummonFxId = app->audio->LoadFx(parameters.child("ghostSummonAudio").attribute("path").as_string());
	lightTexturePath = parameters.attribute("lightTexture").as_string();

	return true;
}

bool Ghost::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pathTexture = app->tex->Load("Assets/Textures/path.png");
	lightTexture = app->tex->Load(lightTexturePath);
	pbody = app->physics->CreateCircle(position.x, position.y, 20, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::GHOST;
	pbody->listener = this;

	summonPbody = app->physics->CreateCircle(summonPosition.x, summonPosition.y, 20, bodyType::KINEMATIC);
	summonPbody->ctype = ColliderType::GHOST_SUMMON;
	summonPbody->body->GetFixtureList()->SetSensor(true);
	summonPbody->listener = this;

	initialTransform = pbody->body->GetTransform();
	initialSummonTransform = summonPbody->body->GetTransform();

	LoadAnimations();

	return true;
}

bool Ghost::Update(float dt)
{
	playerTilePos = app->map->WorldToMap(app->scene->player->position.x + 50, app->scene->player->position.y);

	//ghost
	if (currentAnim == &ghostSummonAnim && currentAnim->HasFinished())
	{
		currentAnim = &ghostIdleAnim;
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

	SDL_Rect rect = currentAnim->GetCurrentFrame();
	if (isFacingRight) app->render->DrawTexture(texture, position.x - 110, position.y - 130, &rect);
	else app->render->DrawTexture(texture, position.x - 110, position.y - 130, &rect, SDL_FLIP_HORIZONTAL);
	currentAnim->Update();
	
	//ghost summon
	summonTilePos = app->map->WorldToMap(summonPosition.x, summonPosition.y);

	if (currentSummonAnim == &ghostSummonAppearAnim && currentSummonAnim->GetCurrentFrameCount() == 1)
	{
		app->audio->PlayFx(ghostSummonFxId);
	}

	distance = playerTilePos.DistanceTo(summonTilePos);

	if (distance < 10)
	{
		app->map->pathfinding->CreatePath(summonTilePos, playerTilePos, true);

		if (distance < 2)
		{
			velocity = { 0, 0 };
			if (isSummonFollowing) isSummonFollowing = false;
		}
		else if (distance >= 2)
		{
			const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
			if (path->Count() > 1) {
				nextTilePath = { path->At(1)->x, path->At(1)->y };
				Move(summonTilePos, nextTilePath);
			}

			if (!isSummonFollowing) isSummonFollowing = true;
		}
		else
		{
			velocity = { 0, 0 };
			app->map->pathfinding->ClearLastPath();
		}
	}

	if (currentSummonAnim == &ghostSummonAppearAnim && currentSummonAnim->HasFinished())
	{
		isSummonFollowing = true;
		currentSummonAnim = &ghostSummonIdleAnim;
	}

	summonPosition.x = METERS_TO_PIXELS(summonPbody->body->GetTransform().p.x);
	summonPosition.y = METERS_TO_PIXELS(summonPbody->body->GetTransform().p.y);

	summonPbody->body->SetLinearVelocity(velocity);

	// draw light
	if (isSummonFacingRight) app->render->DrawTexture(lightTexture, summonPosition.x - 60, summonPosition.y - 50);
	else app->render->DrawTexture(lightTexture, summonPosition.x - 50, summonPosition.y - 50);

	SDL_Rect summonRect = currentSummonAnim->GetCurrentFrame();
	if (isSummonFollowing)
	{
		if (isSummonFacingRight) app->render->DrawTexture(texture, summonPosition.x - 100, summonPosition.y - 90, &summonRect, SDL_FLIP_NONE, 1.0f, -90);
		else app->render->DrawTexture(texture, summonPosition.x - 100, summonPosition.y - 90, &summonRect, SDL_FLIP_HORIZONTAL, 1.0f, 90);
	}
	else
	{
		if (isSummonFacingRight) app->render->DrawTexture(texture, summonPosition.x - 100, summonPosition.y - 90, &summonRect);
		else app->render->DrawTexture(texture, summonPosition.x - 100, summonPosition.y - 90, &summonRect, SDL_FLIP_HORIZONTAL);
	}
	currentSummonAnim->Update();

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

bool Ghost::CleanUp()
{
	return true;
}

void Ghost::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{

	}
}

void Ghost::LoadAnimations()
{
	ghostAttackAnim.LoadAnimations("ghostAttackAnim", "ghost");
	ghostDeadAnim.LoadAnimations("ghostDeadAnim", "ghost");
	ghostIdleAnim.LoadAnimations("ghostIdleAnim", "ghost");
	ghostIdle2Anim.LoadAnimations("ghostIdle2Anim", "ghost");
	ghosSkillAnim.LoadAnimations("ghosSkillAnim", "ghost");
	ghostSummonAnim.LoadAnimations("ghostSummonAnim", "ghost");

	ghostSummonIdleAnim.LoadAnimations("ghostSummonIdleAnim", "ghost");
	ghostSummonAppearAnim.LoadAnimations("ghostSummonAppearAnim", "ghost");
	ghostSummonDeathAnim.LoadAnimations("ghostSummonDeathAnim", "ghost");

	currentAnim = &ghostSummonAnim;
	currentSummonAnim = &ghostSummonAppearAnim;
}

void Ghost::Move(const iPoint& origin, const iPoint& destination) {

	float xDiff = destination.x - origin.x;
	float yDiff = destination.y - origin.y;


	velocity.x = (xDiff < 0) ? -2 : (xDiff > 0) ? 2 : 0;
	velocity.y = (yDiff < 0) ? -2 : (yDiff > 0) ? 2 : 0;

	isSummonFacingRight = (xDiff > 0);

}