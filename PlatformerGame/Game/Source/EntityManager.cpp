#include "EntityManager.h"
#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include "Key.h"
#include "Jumper.h"
#include "CrumblingPlatform.h"
#include "LockDoor.h"
#include "Skeleton.h"
#include "Ghost.h"
#include "Equipment.h"
#include "Obstacle.h"
#include "Chest.h"
#include "LogObstacle.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager(bool enabled) : Module(enabled)
{
	name.Create("entitymanager");
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	entities.Clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::KEY:
		entity = new Key();
		break;
	case EntityType::EQUIPMENT:
		entity = new Equipment();
		break;
	case EntityType::JUMPER:
		entity = new Jumper();
		break;
	case EntityType::CRUMBLING_PLATFORM:
		entity = new CrumblingPlatform();
		break;
	case EntityType::LOCK_DOOR:
		entity = new LockDoor();
		break;
	case EntityType::SKELETON:
		entity = new Skeleton();
		break;
	case EntityType::GHOST:
		entity = new Ghost();
		break;
	case EntityType::OBSTACLE:
		entity = new Obstacle();
		break;
	case EntityType::CHEST:
		entity = new Chest();
		break;
	case EntityType::LOG_OBSTACLE:
		entity = new LogObstacle();
		break;
	default:
		break;
	}

	entities.Add(entity);
	entity->Awake();

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity) entities.Del(item);
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.Add(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Update(dt);
	}

	return ret;
}

void EntityManager::GetEnemies(List<Entity*>& skeletonsList, List<Entity*>& ghostsList) const
{
	skeletonsList.Clear();
	ghostsList.Clear();

	ListItem<Entity*>* entity;

	for (entity = entities.start; entity != NULL; entity = entity->next)
	{
		if (entity->data->type == EntityType::SKELETON)
		{
			skeletonsList.Add(entity->data);
		}
		else if (entity->data->type == EntityType::GHOST)
		{
			ghostsList.Add(entity->data);
		}
	}
}
