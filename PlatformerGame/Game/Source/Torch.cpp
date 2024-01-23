#include "Torch.h"
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

Torch::Torch() : Entity(EntityType::LIFE)
{
	name.Create("torch");
}

Torch::~Torch() {}

bool Torch::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	torchAudioFxId = app->audio->LoadFx(parameters.child("torchAudio").attribute("path").as_string());

	return true;
}

bool Torch::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x, position.y, 40, bodyType::STATIC);
	pbody->ctype = ColliderType::TORCH;
	pbody->body->GetFixtureList()->SetSensor(true);
	pbody->listener = this;

	torchIdleAnim.LoadAnimations("torchIdleAnim", "torch");
	torchFireAnim.LoadAnimations("torchFireAnim", "torch");
	currentAnim = &torchIdleAnim;

	return true;
}

bool Torch::Update(float dt)
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 36;

	if(!isPicked) currentAnim = &torchIdleAnim;
	else {
		currentAnim = &torchFireAnim;
	}

	app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame());

	currentAnim->Update();
	return true;
}

bool Torch::CleanUp()
{
	return true;
}

void Torch::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physA == pbody) {
		switch (physB->ctype)
		{
		case ColliderType::PLAYER:
			LOG("Collision PLAYER");

			if(!isPicked) {
				app->audio->PlayFx(torchAudioFxId);
				isPicked = true;
			}
			break;
		}
	}
}