#include "Obstacle.h"
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

Obstacle::Obstacle() : Entity(EntityType::OBSTACLE)
{
	name.Create("obstacle");
}

Obstacle::~Obstacle() {}

bool Obstacle::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Obstacle::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 32, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::OBSTACLE;
	pbody->listener = this;
	pbody->body->GetFixtureList()->SetFriction(5.0f); // Valor de fricción lineal
	pbody->body->SetFixedRotation(true);

	return true;
}

bool Obstacle::Update(float dt)
{

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	app->render->DrawTexture(texture, position.x - 16, position.y - 32 - 15);

	return true;
}

bool Obstacle::CleanUp()
{
	return true;
}

void Obstacle::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{

	}
}
