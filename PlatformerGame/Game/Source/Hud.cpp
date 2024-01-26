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

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

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
	deadScreenTexture = app->tex->Load("Assets/Textures/Screens/die_screen.png");
	keyTexture = app->tex->Load("Assets/Textures/key.png");

	timer = Timer();
	timer.Start();

	return true;
}

bool Hud::Update(float dt)
{
	if (app->scene->player->health == 100) app->render->DrawTexture(lifeBarTexture, 0, 0, NULL, SDL_FLIP_NONE, 0);
	
	DrawTimer();

	app->render->DrawTexture(keyTexture, 600, 20, &lifeRect, SDL_FLIP_NONE, 0);

	string keyText = to_string(app->scene->player->keys);

	app->render->DrawText(keyText.c_str(), 640, 25, 20, 20);

	string scoreText = "Score: " + to_string(app->scene->player->score);

	app->render->DrawText(scoreText.c_str(), 400, 25, 100, 25);

	if(playerDeadHud && !spacePressed && !app->scene->player->revived) {
		app->render->DrawTexture(deadScreenTexture, 0, 0, NULL, SDL_FLIP_NONE, 0);
		if(app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !spacePressed) {
			spacePressed = true;
			playerDeadHud = false;
		}
	}

	return true;
}

void Hud::DrawTimer()
{
	int time = 100000 - timer.ReadMSec();
	int minutes = time / 60000;
	int seconds = (time / 1000) % 60;

	ostringstream timerTextStream;
	timerTextStream << setfill('0') << setw(2) << minutes << ":"
		<< setfill('0') << setw(2) << seconds;

	string timerTextStr = timerTextStream.str();
	const char* timerTextCStr = timerTextStr.c_str();

	app->render->DrawText(timerTextCStr, 200, 25, 100, 25);
}

bool Hud::CleanUp()
{
	LOG("Freeing Hud");

	return true;
}
