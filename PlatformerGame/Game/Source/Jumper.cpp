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

	/*position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();*/

	return true;
}

bool Jumper::Start() {

	texturePath = parameters.attribute("texturepath").as_string();
	jumperAudio1FxId = app->audio->LoadFx(parameters.child("boingAudio1").attribute("path").as_string());
	jumperAudio2FxId = app->audio->LoadFx(parameters.child("boingAudio2").attribute("path").as_string());

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangle(position.x + 16, position.y + 27, 48, 12, bodyType::STATIC);
	pbody->ctype = ColliderType::JUMP;
	pbody->listener = this;

	jumperIdleAnim.LoadAnimations("jumperIdleAnim", "jumper");
	jumperAnim.LoadAnimations("jumperAnim", "jumper");

	currentAnim = &jumperIdleAnim;

	return true;
}

bool Jumper::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - 32) ;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - 58) ;

	if (isActivated)
	{
		currentAnim->Reset();
		currentAnim = &jumperAnim;
		int random = rand() % 2;
		switch (random)
		{
			case 0:
				app->audio->PlayFx(jumperAudio1FxId);
				break;
			case 1:
				app->audio->PlayFx(jumperAudio2FxId);
				break;

		}
		isActivated = false;
	}

	SDL_Rect rect = currentAnim->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x , position.y, &rect);
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