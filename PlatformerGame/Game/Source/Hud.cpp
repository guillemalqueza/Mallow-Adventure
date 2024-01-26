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
#include "SceneMenu.h"

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

	lifeBarTexture = app->tex->Load("Assets/Textures/health_bar.png");
	deadScreenTexture = app->tex->Load("Assets/Textures/Screens/die_screen.png");
	keyTexture = app->tex->Load("Assets/Textures/key.png");

	pause = app->tex->Load("Assets/Textures/Screens/Pause/pause.png");

	exitNormal = app->tex->Load("Assets/Textures/Screens/More/exit_normal.png");
	exitHover = app->tex->Load("Assets/Textures/Screens/More/exit_hover.png");
	exitClick = app->tex->Load("Assets/Textures/Screens/More/exit_click.png");
	resumeNormal = app->tex->Load("Assets/Textures/Screens/Pause/resume_normal.png");
	resumeHover = app->tex->Load("Assets/Textures/Screens/Pause/resume_hover.png");
	resumeClick = app->tex->Load("Assets/Textures/Screens/Pause/resume_click.png");
	settingsNormal = app->tex->Load("Assets/Textures/Screens/Pause/settings_normal.png");
	settingsHover = app->tex->Load("Assets/Textures/Screens/Pause/settings_hover.png");
	settingsClick = app->tex->Load("Assets/Textures/Screens/Pause/settings_click.png");
	backToTitleNormal = app->tex->Load("Assets/Textures/Screens/Pause/back_to_title_normal.png");
	backToTitleHover = app->tex->Load("Assets/Textures/Screens/Pause/back_to_title_hover.png");
	backToTitleClick = app->tex->Load("Assets/Textures/Screens/Pause/back_to_title_click.png");

	exitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, NULL, exitNormal, exitHover, exitClick, { 1419, 92, 63, 63 }, this);
	resumeButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, NULL, resumeNormal, resumeHover, resumeClick, { 657, 305, 281, 64 }, this);
	settingsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, NULL, settingsNormal, settingsHover, settingsClick, { 657, 418, 279, 64 }, this);
	backToTitleButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, NULL, backToTitleNormal, backToTitleHover, backToTitleClick, { 599, 531, 399, 64 }, this);

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

	if (app->scene->pause) {

		if (resumeButton->state == GuiControlState::HIDDEN)
		{
			resumeButton->state = GuiControlState::NORMAL;
			settingsButton->state = GuiControlState::NORMAL;
			backToTitleButton->state = GuiControlState::NORMAL;
			exitButton->state = GuiControlState::NORMAL;
		}

		app->render->DrawTexture(pause, 0, 0, NULL, SDL_FLIP_NONE, 0);
		if (resumeButton->state == GuiControlState::FOCUSED) {
			if (app->sceneMenu->fxHoverPlayed == false)
			{
				app->audio->PlayFx(app->sceneMenu->buttonFxHover);
				app->sceneMenu->fxHoverPlayed = true;
			}
		}
		else if (resumeButton->state == GuiControlState::PRESSED)
		{
			if (app->sceneMenu->fxClickPlayed == false)
			{
				app->audio->PlayFx(app->sceneMenu->buttonFxClick);
				app->sceneMenu->fxClickPlayed = true;
			}
			app->scene->pause = false;
		}
		else if (settingsButton->state == GuiControlState::FOCUSED) {
			if (app->sceneMenu->fxHoverPlayed == false)
			{
				app->audio->PlayFx(app->sceneMenu->buttonFxHover);
				app->sceneMenu->fxHoverPlayed = true;
			}
		}
		else if (settingsButton->state == GuiControlState::PRESSED)
		{
			if (app->sceneMenu->fxClickPlayed == false)
			{
				app->audio->PlayFx(app->sceneMenu->buttonFxClick);
				app->sceneMenu->fxClickPlayed = true;
			}
			app->scene->pause = false;
			//app->sceneMenu->settings = true;
		}
		else if (backToTitleButton->state == GuiControlState::FOCUSED) {
			if (app->sceneMenu->fxHoverPlayed == false)
			{
				app->audio->PlayFx(app->sceneMenu->buttonFxHover);
				app->sceneMenu->fxHoverPlayed = true;
			}
		}
		else if (backToTitleButton->state == GuiControlState::PRESSED)
		{
			if (app->sceneMenu->fxClickPlayed == false)
			{
				app->audio->PlayFx(app->sceneMenu->buttonFxClick);
				app->sceneMenu->fxClickPlayed = true;
			}
			app->scene->pause = false;
			//app->fade->FadeToBlack(this, (Module*)app->sceneMenu, 90);
		}
		else if (exitButton->state == GuiControlState::FOCUSED) {
			if (app->sceneMenu->fxHoverPlayed == false)
			{
				app->audio->PlayFx(app->sceneMenu->buttonFxHover);
				app->sceneMenu->fxHoverPlayed = true;
			}
		}
		else if (exitButton->state == GuiControlState::PRESSED)
		{
			if (app->sceneMenu->fxClickPlayed == false)
			{
				app->audio->PlayFx(app->sceneMenu->buttonFxClick);
				app->sceneMenu->fxClickPlayed = true;
			}
			SDL_Quit();
		}
		else {
			app->sceneMenu->fxHoverPlayed = false;
			app->sceneMenu->fxClickPlayed = false;
		}
	}
	else
	{
		resumeButton->state = GuiControlState::HIDDEN;
		settingsButton->state = GuiControlState::HIDDEN;
		backToTitleButton->state = GuiControlState::HIDDEN;
		exitButton->state = GuiControlState::HIDDEN;
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
