#include "Jumper.h"
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

Jumper::Jumper() : Entity(EntityType::JUMPER)
{
	name.Create("jumper");
}

Jumper::~Jumper() {}

bool Jumper::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Jumper::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangle(position.x, position.y, 32, 10, bodyType::STATIC);
	pbody->ctype = ColliderType::JUMP;
	pbody->listener = this;

	idleAnim.PushBack({ 0,0,32,32 });
	idleAnim.loop = false;
	idleAnim.speed = 0.1f;

	jumpAnim.PushBack({ 0,0,32,32 });
	jumpAnim.PushBack({ 32,0,32,32 });
	jumpAnim.PushBack({ 64,0,32,32 });
	jumpAnim.PushBack({ 96,0,32,32 });
	jumpAnim.PushBack({ 128,0,32,32 });
	jumpAnim.PushBack({ 160,0,32,32 });
	jumpAnim.PushBack({ 0,0,32,32 });
	jumpAnim.loop = false;
	jumpAnim.speed = 0.25f;

	currentAnim = &idleAnim;

	return true;
}

bool Jumper::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 15;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 27;

	if (isActivated)
	{
		currentAnim->Reset();
		currentAnim = &jumpAnim;
		isActivated = false;
	}

	SDL_Rect rect = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect);
	currentAnim->Update();

	return true;
}

bool Jumper::CleanUp()
{
	return true;
}

void Jumper::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		isActivated = true;
		break;
	}
}