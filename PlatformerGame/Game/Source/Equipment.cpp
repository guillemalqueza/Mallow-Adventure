#include "Equipment.h"
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

Equipment::Equipment() : Entity(EntityType::EQUIPMENT)
{
	name.Create("equipment");
}

Equipment::~Equipment() {}

bool Equipment::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Equipment::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangleSensor(position.x + 90, position.y + 190, 25, 200, bodyType::STATIC);
	pbody->ctype = ColliderType::EQUIPMENT;
	pbody->listener = this;

	pbody2 = app->physics->CreateCircle(position.x + 90, position.y + 190, 250, bodyType::STATIC);
	pbody2->body->GetFixtureList()->SetSensor(true);
	pbody2->ctype = ColliderType::EQUIPMENT_AREA;
	pbody2->listener = this;

	equipmentWithSword.LoadAnimations("equipmentWithSword", "equipment");
	equipmentAnim.LoadAnimations("equipmentAnim", "equipment");
	equipmentWithoutSword.LoadAnimations("equipmentWithoutSword", "equipment");	
	equipmentAnimIdle.LoadAnimations("equipmentAnimIdle", "equipment");
	currentAnim = &equipmentWithSword;


	return true;
}

bool Equipment::Update(float dt)
{
	if (isPicked)	OnPicked();
	if (equipmentArea) { 
		currentAnim = &equipmentAnim;
		OnSensor();
	}
	if (currentAnim == &equipmentAnim && currentAnim->HasFinished())
	{
		currentAnim= &equipmentAnimIdle;
	}


	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 90;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 190;

	app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame());

	currentAnim->Update();
	return true;
}

bool Equipment::CleanUp()
{
	return true;
}

void Equipment::OnCollision(PhysBody* physA, PhysBody* physB)
{
	
	
}

void Equipment::OnPicked()
{
	currentAnim = &equipmentWithoutSword;
	//app->entityManager->DestroyEntity(this);
	//app->physics->world->DestroyBody(pbody->body);
	isPicked = false;
}

void Equipment::OnSensor()
{
	app->physics->world->DestroyBody(pbody2->body);
	equipmentArea = false;
}