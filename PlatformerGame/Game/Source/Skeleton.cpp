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
	pbody = app->physics->CreateCircle(position.x, position.y, 20, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::SKELETON;
	pbody->listener = this;

	initialTransform = pbody->body->GetTransform();

	LoadAnimations();

	return true;
}

bool Skeleton::Update(float dt)
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

	SDL_Rect rect = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x-40, position.y-40, &rect);
	currentAnim->Update();

	return true;
}

bool Skeleton::CleanUp()
{
	return true;
}

void Skeleton::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{

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