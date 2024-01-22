#include "Hud.h"

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
#include "ParticleManager.h"

#include "Defs.h"
#include "Log.h"

Hud::Hud(bool enabled) : Module(enabled)
{
	name.Create("scenemenu");
}

Hud::~Hud()
{}

bool Hud::Awake(pugi::xml_node& config)
{
	LOG("Loading SceneMenu");
	bool ret = true;


	return ret;
}

bool Hud::Start()
{

	lifeBarTexture = app->tex->Load("Assets/Textures/ghost_shadow.png");

	return true;
}

bool Hud::Update(float dt)
{
	if (app->scene->player->health == 100) app->render->DrawTexture(lifeBarTexture, 0, 0, NULL, SDL_FLIP_NONE, 0);
	LOG("Health: %d", app->scene->player->health);
	return true;
}

bool Hud::CleanUp()
{
	LOG("Freeing Hud");


	return true;
}
