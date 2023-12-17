#include "LogObstacle.h"
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
#include "FadeToBlack.h"

LogObstacle::LogObstacle() : Entity(EntityType::LOG_OBSTACLE)
{
	name.Create("log");
}

LogObstacle::~LogObstacle() {}

bool LogObstacle::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool LogObstacle::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangle(position.x + 240, position.y + 200, 25, 130, bodyType::DYNAMIC);
	pbody->body->SetGravityScale(0);
	pbody->ctype = ColliderType::LOG_OBSTACLE;
	pbody->listener = this;

	logIdleAnim.LoadAnimations("logIdleAnim", "log");
	logDownAnim.LoadAnimations("logDownAnim", "log");
	logDownIdle.LoadAnimations("logDownIdle", "log");
	currentAnim = &logIdleAnim;

	return true;
}

bool LogObstacle::Update(float dt)
{
	if(app->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN) health -= 1;
	if (health <= 0 && logUp) {
		currentAnim= &logDownAnim;
		logUp = false;
		LOG("LOG DOWN");	
	}
	if (!logUp && currentAnim == &logDownAnim && currentAnim->HasFinished()) {
		pbody->body->SetActive(false);
		currentAnim = &logDownIdle;
		LOG("LOG DOWN IDLE");
	}

	if (!logUp && app->scene->player->position.x > (position.x + 100) && !changeLevel)
	{
		app->fade->Fade(3, 60);
		changeLevel = true;
	}

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 124;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 120;

	app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame());

	currentAnim->Update();
	return true;
}

bool LogObstacle::CleanUp()
{
	return true;
}

void LogObstacle::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
		case ColliderType::SWORD:
		health -= 1;
		break;
	}
	
}