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

	if (config.child("player")) {
		player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
		player->parameters = config.child("player");
	}

	CreateEntities(config, "item", EntityType::ITEM);
	CreateEntities(config, "lockDoor", EntityType::LOCK_DOOR);
	CreateEntities(config, "jumper", EntityType::JUMPER);
	CreateEntities(config, "crumblingPlatform", EntityType::CRUMBLING_PLATFORM);

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
	app->render->DrawTexture(backgroundTexture, 0, 0, &bg, SDL_FLIP_NONE, 0.0f);

	playerX = player->position.x;
	playerY = player->position.y;

	if (cameraIdx == 0) SetCameraPosition(56, 760 - (windowH / 2));
	else if (cameraIdx == 1) SetCameraPosition(2460 - (windowW / 2), 575 - (windowH / 2));

	ClampCamera();

	if (app->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) StartCameraShakeX(20.0f, 5.0f);

	if (app->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN) StartCameraShakeY(20.0f, 5.0f);

	UpdateCameraShake();
	
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
	if (cameraX < 0) cameraX = 0;
	else if (cameraX + windowW > levelWidth) cameraX = levelWidth - windowW;

	if (cameraY < 0) cameraY = 0;
	else if (cameraY + windowH > levelHeight) cameraY = levelHeight - windowH;
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

