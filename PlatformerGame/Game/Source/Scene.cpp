#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");

	// ground
	bg.x = 0;
	bg.y = 0;
	bg.w = 1680;
	bg.h = 1050;
}

// Destructor
Scene::~Scene()
{}

pugi::xml_node configNode;

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access

	CreateEntities(config, "key", EntityType::KEY);
	CreateEntities(config, "equipment", EntityType::EQUIPMENT);
	CreateEntities(config, "lockDoor", EntityType::LOCK_DOOR);
	CreateEntities(config, "jumper", EntityType::JUMPER);
	CreateEntities(config, "crumblingPlatform", EntityType::CRUMBLING_PLATFORM);
	CreateEntities(config, "skeleton", EntityType::SKELETON);
	CreateEntities(config, "ghost", EntityType::GHOST);
	CreateEntities(config, "obstacle", EntityType::OBSTACLE);

	if (config.child("player")) {
		player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
		player->parameters = config.child("player");
	}

	if (config.child("map")) {
		//Get the map name from the config file and assigns the value in the module
		app->map->name = config.child("map").attribute("name").as_string();
		app->map->path = config.child("map").attribute("path").as_string();
	}

	configNode = config;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	// NOTE: We have to avoid the use of paths in the code, we will move it later to a config file
	backgroundTexture = app->tex->Load(configNode.child("background").attribute("texturepath").as_string());
	backgroundTexture2 = app->tex->Load(configNode.child("background2").attribute("texturepath").as_string());
	//Music is commented so that you can add your own music
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	app->tex->GetSize(backgroundTexture, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	app->render->camera.x = -32;
	app->render->camera.y = -286;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if (!level2Enabled) app->render->DrawTexture(backgroundTexture, 0, 0, &bg, SDL_FLIP_NONE, 0.0f);
	else app->render->DrawTexture(backgroundTexture2, 0, 0, &bg, SDL_FLIP_NONE, 0.0f);

	playerX = player->position.x;
	playerY = player->position.y;

	if (cameraIdx == 0) SetCameraPosition(56, 760 - (windowH / 2));
	else if (cameraIdx == 1) SetCameraPosition(2460 - (windowW / 2), 575 - (windowH / 2));
	else if (cameraIdx == 2) SetCameraPosition(player->position.x - (windowW / 2), player->position.y + 60 - (windowH / 2));
	else if (cameraIdx == 3) SetCameraPosition(100, 5240);
	else if (cameraIdx == 4) SetCameraPosition(1100, 4400);
	else if (cameraIdx == 5) SetCameraPosition(2000, 3520);
	else if (cameraIdx == 6) SetCameraPosition(3032, 2680);
	else if (cameraIdx == 7) SetCameraPosition(2800, 1800);
	else if (cameraIdx == 8) SetCameraPosition(5000, 1000);
	else if (cameraIdx == 9) SetCameraPosition(6000, 200);

	ClampCamera();

	if (app->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) StartCameraShakeX(20.0f, 5.0f);

	if (app->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN) StartCameraShakeY(20.0f, 5.0f);

	UpdateCameraShake();
	
	if (cameraInitialized && !changingLevel)
	{
		app->render->camera.x += (-cameraX - app->render->camera.x) * cameraSmoothingFactor;
		app->render->camera.y += (-cameraY - app->render->camera.y) * cameraSmoothingFactor;
	}
	else if (changingLevel)
	{
		app->render->camera.x = -cameraX;
		app->render->camera.y = -cameraY;
		changingLevel = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) app->fade->Fade(1,60);
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) app->fade->Fade(2, 60);
	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) app->fade->Fade(3, 60);

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) app->SaveRequest();
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) app->LoadRequest();


	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	app->map->Disable();
	//app->entityManager->DestroyEntity(player);
	return true;
}

void Scene::CreateEntities(pugi::xml_node& config, const char* nodeName, EntityType entityType)
{
	for (pugi::xml_node entityNode = config.child(nodeName); entityNode; entityNode = entityNode.next_sibling(nodeName))
	{
		Entity* entity = app->entityManager->CreateEntity(entityType);
		entity->parameters = entityNode;
	}
}

void Scene::SetCameraPosition(int x, int y)
{
	cameraX = x;
	cameraY = y;
}

void Scene::ClampCamera()
{
	if (level1Enabled)
	{
		if (cameraX < 0) cameraX = 0;
		else if (cameraX + windowW > levelWidth) cameraX = levelWidth - windowW;

		if (cameraY < 0) cameraY = 0;
		else if (cameraY + windowH > levelHeight) cameraY = levelHeight - windowH;
	}
	else if (level2Enabled)
	{
		if (cameraX < levelWidth + 550) cameraX = levelWidth + 550;
		else if (cameraX + windowW > levelWidth + level2Width + 300) cameraX = (levelWidth +  level2Width + 300) - windowW;

		if (cameraY < 0) cameraY = 0;
		else if (cameraY > 200) cameraY = 460;
		else if (cameraY + windowH > level2Height) cameraY = level2Height - windowH;
	}
	else if (level3Enabled)
	{
		
	}
}

void Scene::StartCameraShakeX(float duration, float intensity)
{
	shakingCameraX = true;
	shakeTimer = duration;
	shakeIntensity = intensity;
	cameraInitialized = false;
}

void Scene::StartCameraShakeY(float duration, float intensity)
{
	shakingCameraY = true;
	shakeTimer = duration;
	shakeIntensity = intensity;
	cameraInitialized = false;
}

void Scene::UpdateCameraShake()
{
	if (shakingCameraX)
	{
		if (shakeTimer > 0)
		{
			float offsetX = sin(shakeTimer * 20.0f) * shakeIntensity;
			app->render->camera.x += static_cast<int>(offsetX);
			shakeTimer -= 1.0f;
		}
		else shakingCameraX = false;
	}
	else if (shakingCameraY)
	{
		if (shakeTimer > 0)
		{
			float offsetY = sin(shakeTimer * 20.0f) * shakeIntensity;
			app->render->camera.y += static_cast<int>(offsetY);
			shakeTimer -= 1.0f;
		}
		else shakingCameraY = false;
	}
	else cameraInitialized = true;
}

void Scene::StartLevel1()
{
	player->pbody->body->SetTransform(b2Vec2(player->initialTransform.p.x, player->initialTransform.p.y), 0);
	if (cameraIdx != 0 && cameraIdx != 1) changingLevel = true;
	cameraInitialized = true;
	cameraIdx = 0;
	cameraInitialized = true;
	level1Enabled = true;
	level2Enabled = false;
	level3Enabled = false;
	player->isDead = false;
	app->map->mapIdx = 1;
	app->map->UpdateMapSize();
}

void Scene::StartLevel2()
{
	player->pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(4120), PIXEL_TO_METERS(830)), 0);
	if (cameraIdx != 2) changingLevel = true;
	cameraInitialized = true;
	cameraIdx = 2;
	level2Enabled = true;
	level1Enabled = false;
	level3Enabled = false;
	player->isDead = false;
	app->map->mapIdx = 2;
	app->map->UpdateMapSize();
}

void Scene::StartLevel3()
{
	player->pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(320), PIXEL_TO_METERS(5824)), 0);
	if (cameraIdx != 3) changingLevel = true;
	cameraInitialized = true;
	cameraIdx = 3;
	level3Enabled = true;
	level1Enabled = false;
	level2Enabled = false;
	player->isDead = false;
	app->map->mapIdx = 3;
	app->map->UpdateMapSize();
}	

