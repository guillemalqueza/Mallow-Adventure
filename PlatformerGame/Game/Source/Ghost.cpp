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

	return true;
}

bool Ghost::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x, position.y, 20, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::GHOST;
	pbody->listener = this;

	initialTransform = pbody->body->GetTransform();

	LoadAnimations();

	return true;
}

bool Ghost::Update(float dt)
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

	SDL_Rect rect = currentAnim->GetCurrentFrame();
	if (isFacingRight) app->render->DrawTexture(texture, position.x - 90, position.y - 130, &rect);
	else app->render->DrawTexture(texture, position.x - 110, position.y - 130, &rect, SDL_FLIP_HORIZONTAL);
	currentAnim->Update();

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

	currentAnim = &ghostIdleAnim;
}