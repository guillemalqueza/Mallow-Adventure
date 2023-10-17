#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"

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

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	if (config.child("player")) {
		player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
		player->parameters = config.child("player");
	}

	if (config.child("jumper")) {
		Jumper* jumper = (Jumper*)app->entityManager->CreateEntity(EntityType::JUMPER);
		jumper->parameters = config.child("jumper");
	}

	if (config.child("crumblingPlatform")) {
		CrumblingPlatform* crumblingPlatform = (CrumblingPlatform*)app->entityManager->CreateEntity(EntityType::CRUMBLING_PLATFORM);
		crumblingPlatform->parameters = config.child("crumblingPlatform");
	}

	if (config.child("lockDoor")) {
		LockDoor* lockDoor = (LockDoor*)app->entityManager->CreateEntity(EntityType::LOCK_DOOR);
		lockDoor->parameters = config.child("lockDoor");
	}

	if (config.child("map")) {
		//Get the map name from the config file and assigns the value in the module
		app->map->name = config.child("map").attribute("name").as_string();
		app->map->path = config.child("map").attribute("path").as_string();
	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	// NOTE: We have to avoid the use of paths in the code, we will move it later to a config file
	backgroundTexture = app->tex->Load("Assets/Maps/background.png");
	
	//Music is commented so that you can add your own music
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	app->tex->GetSize(backgroundTexture, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->render->camera.x = 0;
	app->render->camera.y = -110;
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
	app->render->DrawTexture(backgroundTexture, 0, 0, &bg, SDL_FLIP_NONE, 0.0f);

	playerX = player->position.x;
	playerY = player->position.y;

	if (cameraIdx == 0)
	{
		cameraX = 400 - (windowW / 2);
		cameraY = 659 - (windowH / 2);
	}
	else if (cameraIdx == 1)
	{
		cameraX = 2460 - (windowW / 2);
		cameraY = 179 - (windowH / 2);
	}

	if (cameraX < 0) {
		cameraX = 0;
	}
	else if (cameraX + windowW > levelWidth) {
		cameraX = levelWidth - windowW;
	}

	if (cameraY < 0) {
		cameraY = 0;
	}
	else if (cameraY + windowH > levelHeight) {
		cameraY = levelHeight - windowH;
	}

	if (app->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		shakingCameraX = true;
		shakeDuration = 20.0f;
		shakeIntensity = 5.0f;
		shakeTimer = shakeDuration;
		cameraInitialized = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
	{
		shakingCameraY = true;
		shakeDuration = 20.0f;
		shakeIntensity = 5.0f;
		shakeTimer = shakeDuration;
		cameraInitialized = false;
	}

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
	
	if (cameraInitialized)
	{
		app->render->camera.x += (-cameraX - app->render->camera.x) * cameraSmoothingFactor;
		app->render->camera.y += (-cameraY - app->render->camera.y) * cameraSmoothingFactor;
	}
	

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

	return true;
}
