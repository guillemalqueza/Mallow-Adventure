#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "FadeToBlack.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

using namespace std;

Scene::Scene(bool enabled) : Module(enabled)
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

	//CreateEntities(config, "key", EntityType::KEY);
	//CreateEntities(config, "equipment", EntityType::EQUIPMENT);
	//CreateEntities(config, "lockDoor", EntityType::LOCK_DOOR);
	//CreateEntities(config, "jumper", EntityType::JUMPER);
	CreateEntities(config, "crumblingPlatform", EntityType::CRUMBLING_PLATFORM);
	CreateEntities(config, "skeleton", EntityType::SKELETON);
	CreateEntities(config, "ghost", EntityType::GHOST);
	CreateEntities(config, "obstacle", EntityType::OBSTACLE);
	CreateEntities(config, "chest", EntityType::CHEST);
	CreateEntities(config, "life", EntityType::LIFE);
	CreateEntities(config, "torch", EntityType::TORCH);
	CreateEntities(config, "log", EntityType::LOG_OBSTACLE);

	app->entityManager->GetEnemies(skeletonsList, ghostsList);

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

	level1Music = config.child("music").child("level1Music").attribute("path").as_string();
	level2Music = config.child("music").child("level2Music").attribute("path").as_string();
	level3Music = config.child("music").child("level3Music").attribute("path").as_string();

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	// NOTE: We have to avoid the use of paths in the code, we will move it later to a config file
	backgroundTexture = app->tex->Load(configNode.child("background").attribute("texturepath").as_string());
	backgroundTexture2 = app->tex->Load(configNode.child("background2").attribute("texturepath").as_string());


	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	app->tex->GetSize(backgroundTexture, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	app->render->camera.x = -32;
	app->render->camera.y = -286;

	app->audio->PlayMusic(level1Music, 0.0f);

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

	// Camera positions
	if (cameraIdx == 0) SetCameraPosition(56, 760 - (windowH / 2));
	else if (cameraIdx == 1) SetCameraPosition(2460 - (windowW / 2), 575 - (windowH / 2));
	else if (cameraIdx == 2) SetCameraPosition(player->position.x - (windowW / 2), player->position.y + 60 - (windowH / 2));
	else if (cameraIdx == 3) SetCameraPosition(100, 5240);
	else if (cameraIdx == 4) SetCameraPosition(1100, 4400);
	else if (cameraIdx == 5) SetCameraPosition(2000, 3520);
	else if (cameraIdx == 6) SetCameraPosition(3032, 2680);
	else if (cameraIdx == 7) SetCameraPosition(2800, 1800);
	else if (cameraIdx == 8) SetCameraPosition(3700, 1800);
	else if (cameraIdx == 9) SetCameraPosition(4900, 1700);

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

	if ((app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN || levelToLoadIdx == 1) && app->fade->fadeFinished)
	{
		levelToLoadIdx = 0;
		if (!isLoading)
		{
			player->wallLeft = false;
			player->wallRight = false;
			newCameraIdx = 0;
			level1SpawnPoint = { 400, 991 };
		}
		app->fade->Fade(1, 60);
		isLoading = false;
	}

	if ((app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN || levelToLoadIdx == 2) && app->fade->fadeFinished)
	{
		levelToLoadIdx = 0;
		if (!isLoading)
		{
			player->wallLeft = false;
			player->wallRight = false;
			newCameraIdx = 2;
			level2SpawnPoint = { 4120, 830 };
		}
		app->fade->Fade(2, 60);
		isLoading = false;
	}
	if ((app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN || levelToLoadIdx == 3) && app->fade->fadeFinished)
	{
		levelToLoadIdx = 0;
		
		if (!isLoading)
		{	
			player->wallLeft = false;
			player->wallRight = false;
			newCameraIdx = 3;		
			level3SpawnPoint = { 320, 5824 };
		}
		app->fade->Fade(3, 60);
		isLoading = false;
	}

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
	// Clamp camera
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
	// Update camera shake
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
	// start level 1
	player->enterDoor = false;
	player->pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(level1SpawnPoint.x + 1), PIXEL_TO_METERS(level1SpawnPoint.y)), 0);
	if (cameraIdx != 0 && cameraIdx != 1) changingLevel = true;
	cameraInitialized = true;
	cameraIdx = newCameraIdx;
	level1Enabled = true;
	level2Enabled = false;
	level3Enabled = false;
	player->isDead = false;
	app->map->mapIdx = 1;
	app->map->UpdateMapSize();
	app->audio->PlayMusic("Assets/Audio/Music/prologue.ogg", 0.0f);
}

void Scene::StartLevel2()
{
	// start level 2
	player->enterDoor = false;
	player->pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(level2SpawnPoint.x + 1), PIXEL_TO_METERS(level2SpawnPoint.y + 1)), 0);
	if (cameraIdx != 2) changingLevel = true;
	cameraInitialized = true;
	cameraIdx = newCameraIdx;
	level2Enabled = true;
	level1Enabled = false;
	level3Enabled = false;
	player->isDead = false;
	app->map->mapIdx = 2;
	app->map->UpdateMapSize();
	app->audio->PlayMusic(level2Music, 0.0f);
}

void Scene::StartLevel3()
{
	// start level 3
	player->enterDoor = false;
	player->pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(level3SpawnPoint.x + 1), PIXEL_TO_METERS(level3SpawnPoint.y + 1)), 0);
	if (cameraIdx != 3) changingLevel = true;
	cameraInitialized = true;
	cameraIdx = newCameraIdx;
	level3Enabled = true;
	level1Enabled = false;
	level2Enabled = false;
	player->isDead = false;
	app->map->mapIdx = 3;
	app->map->UpdateMapSize();
	app->audio->PlayMusic(level3Music, 0.0f);
}	

bool Scene::LoadState(pugi::xml_node node)
{
	//player
	pugi::xml_node playerPositionNode = node.child("player").child("playerPosition");
	player->position.x = playerPositionNode.attribute("x").as_int();
	player->position.y = playerPositionNode.attribute("y").as_int();
	pugi::xml_node playerStatesNode = node.child("player").child("playerStates");
	player->isDead = playerStatesNode.attribute("isDead").as_bool();
	player->isEquipped = playerStatesNode.attribute("isEquipped").as_bool();
	player->wallLeft = playerStatesNode.attribute("wallLeft").as_bool();
	player->wallRight = playerStatesNode.attribute("wallRight").as_bool();
	player->ground = playerStatesNode.attribute("ground").as_bool();
	pugi::xml_node playerItemsNode = node.child("player").child("playerItems");
	player->keys = playerItemsNode.attribute("keys").as_int();

	//scene
	pugi::xml_node levelNode = node.child("sceneConfig").child("currentLevel");
	level1Enabled = levelNode.attribute("level1").as_bool();
	level2Enabled = levelNode.attribute("level2").as_bool();
	level3Enabled = levelNode.attribute("level3").as_bool();
	pugi::xml_node cameraNode = node.child("sceneConfig").child("cameraProperties");
	cameraX = cameraNode.attribute("cameraX").as_int();
	cameraY = cameraNode.attribute("cameraY").as_int();
	cameraIdx = cameraNode.attribute("cameraIdx").as_int();
	cameraInitialized = cameraNode.attribute("cameraInitialized").as_bool();
	newCameraIdx = cameraNode.attribute("cameraIdx").as_int();

	//enemies
	for (int skeletonCount = 0; skeletonCount < skeletonsList.Count(); skeletonCount++) {

		Entity* skeleton = skeletonsList.At(skeletonCount)->data;

		pugi::xml_node skeletonPositionNode = node.child(("skeleton" + to_string(skeletonCount + 1)).c_str()).child("skeletonPosition");
		skeleton->position.x = skeletonPositionNode.attribute("x").as_int();
		skeleton->position.y = skeletonPositionNode.attribute("y").as_int();
		pugi::xml_node skeletonStatesNode = node.child(("skeleton" + to_string(skeletonCount + 1)).c_str()).child("skeletonStates");
		skeleton->isDead = skeletonStatesNode.attribute("isDead").as_bool();
		skeleton->health = skeletonStatesNode.attribute("health").as_int();

		if (!skeleton->isDead) skeleton->ResetEntity();
		skeleton->setLoadPosition = true;
	}

	for (int ghostsCount = 0; ghostsCount < ghostsList.Count(); ghostsCount++) {

		Entity* ghost = ghostsList.At(ghostsCount)->data;

		pugi::xml_node skeletonPositionNode = node.child(("ghost" + to_string(ghostsCount + 1)).c_str()).child("ghostPosition");
		ghost->position.x = skeletonPositionNode.attribute("x").as_int();
		ghost->position.y = skeletonPositionNode.attribute("y").as_int();
		pugi::xml_node skeletonStatesNode = node.child(("ghost" + to_string(ghostsCount + 1)).c_str()).child("ghostStates");
		ghost->isDead = skeletonStatesNode.attribute("isDead").as_bool();
		ghost->health = skeletonStatesNode.attribute("health").as_int();

		if (!ghost->isDead) ghost->ResetEntity();
		ghost->setLoadPosition = true;
	}

	if (level1Enabled) level1SpawnPoint = player->position;
	else if (level2Enabled) level2SpawnPoint = player->position;
	else if (level3Enabled) level3SpawnPoint = player->position;

	isLoading = true;
	player->SetToInitialPosition();

	return true;
}

bool Scene::SaveState(pugi::xml_node node)
{
	//player
	pugi::xml_node playerPositionNode = node.append_child("player").append_child("playerPosition");
	playerPositionNode.append_attribute("x").set_value(player->position.x);
	playerPositionNode.append_attribute("y").set_value(player->position.y);
	pugi::xml_node playerStatesNode = node.child("player").append_child("playerStates");
	playerStatesNode.append_attribute("isDead").set_value(player->isDead);
	playerStatesNode.append_attribute("isEquipped").set_value(player->isEquipped);
	playerStatesNode.append_attribute("wallLeft").set_value(player->wallLeft);
	playerStatesNode.append_attribute("wallRight").set_value(player->wallRight);
	playerStatesNode.append_attribute("ground").set_value(player->ground);
	pugi::xml_node playerItemsNode = node.child("player").append_child("playerItems");
	playerItemsNode.append_attribute("keys").set_value(player->keys);

	//scene
	pugi::xml_node levelNode = node.append_child("sceneConfig").append_child("currentLevel");
	levelNode.append_attribute("level1").set_value(level1Enabled);
	levelNode.append_attribute("level2").set_value(level2Enabled);
	levelNode.append_attribute("level3").set_value(level3Enabled);
	pugi::xml_node cameraNode = node.child("sceneConfig").append_child("cameraProperties");
	cameraNode.append_attribute("cameraX").set_value(cameraX);
	cameraNode.append_attribute("cameraY").set_value(cameraY);
	cameraNode.append_attribute("cameraIdx").set_value(cameraIdx);
	cameraNode.append_attribute("cameraInitialized").set_value(cameraInitialized);
	cameraNode.append_attribute("cameraIdx").set_value(cameraIdx);

	//enemies
	for (int skeletonCount = 0; skeletonCount < skeletonsList.Count(); skeletonCount++) {

		Entity* skeleton = skeletonsList.At(skeletonCount)->data;

		pugi::xml_node skeletonPositionNode = node.append_child(("skeleton" + to_string(skeletonCount + 1)).c_str()).append_child("skeletonPosition");
		skeletonPositionNode.append_attribute("x").set_value(skeleton->position.x);
		skeletonPositionNode.append_attribute("y").set_value(skeleton->position.y);
		pugi::xml_node skeletonStatesNode = node.child(("skeleton" + to_string(skeletonCount + 1)).c_str()).append_child("skeletonStates");
		skeletonStatesNode.append_attribute("isDead").set_value(skeleton->isDead);
		skeletonStatesNode.append_attribute("health").set_value(skeleton->health);
	}

	for (int ghostCount = 0; ghostCount < ghostsList.Count(); ghostCount++) {

		Entity* ghost = ghostsList.At(ghostCount)->data;

		pugi::xml_node ghostPositionNode = node.append_child(("ghost" + to_string(ghostCount + 1)).c_str()).append_child("ghostPosition");
		ghostPositionNode.append_attribute("x").set_value(ghost->position.x);
		ghostPositionNode.append_attribute("y").set_value(ghost->position.y);
		pugi::xml_node ghostStatesNode = node.child(("ghost" + to_string(ghostCount + 1)).c_str()).append_child("ghostStates");
		ghostStatesNode.append_attribute("isDead").set_value(ghost->isDead);
		ghostStatesNode.append_attribute("health").set_value(ghost->health);
	}

	return true;
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: TODO 5: Implement the OnGuiMouseClickEvent method
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	return true;
}

