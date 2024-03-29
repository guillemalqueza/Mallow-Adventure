#include "CrumblingPlatform.h"
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

CrumblingPlatform::CrumblingPlatform() : Entity(EntityType::CRUMBLING_PLATFORM)
{
	name.Create("crumblingPlatform");
}

CrumblingPlatform::~CrumblingPlatform() {}

bool CrumblingPlatform::Awake() {

	/*position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();*/

	return true;
}

bool CrumblingPlatform::Start() {

	texturePath = parameters.attribute("texturepath").as_string();
	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangle(position.x, position.y, 32, 32, bodyType::STATIC);
	pbody->ctype = ColliderType::PLATFORM;
	pbody->listener = this;

	platformIdleAnim.LoadAnimations("platformIdleAnim", "platform");
	platformBreakingAnim.LoadAnimations("platformBreakingAnim", "platform");

	currentAnim = &platformIdleAnim;

	return true;
}

bool CrumblingPlatform::Update(float dt)
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	if (isActivated)
	{
		timer.Start();
		isActivated = false;
		isTimerStarted = true;
		currentAnim = &platformBreakingAnim;
		hasShaked = false;
	}

	if (isTimerStarted)
	{
		if (timer.ReadMSec() >= 1000)
		{
			if (!hasShaked)
			{
				app->scene->StartCameraShakeX(20.0f, 5.0f);
				hasShaked = true;
			}
			pbody->body->SetActive(false);
			isVisible = false;
		}

		if (timer.ReadMSec() >= 2500)
		{
			pbody->body->SetActive(true);
			isVisible = true;
			isTimerStarted = false;
			currentAnim = &platformIdleAnim;
		}
	}

	SDL_Rect rect = currentAnim->GetCurrentFrame();
	if (isVisible) app->render->DrawTexture(texture, position.x, position.y, &rect);
	currentAnim->Update();

	return true;
}

bool CrumblingPlatform::CleanUp()
{
	app->tex->UnLoad(texture);
	return true;
}

void CrumblingPlatform::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		isActivated = true;
		break;
	}
}