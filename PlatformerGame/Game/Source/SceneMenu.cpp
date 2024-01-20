#include "SceneMenu.h"

#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "Chest.h"
#include "FadeToBlack.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

SceneMenu::SceneMenu(bool enabled) : Module(enabled)
{
	name.Create("scenemenu");
}

SceneMenu::~SceneMenu()
{}

bool SceneMenu::Awake(pugi::xml_node& config)
{
	LOG("Loading SceneMenu");
	bool ret = true;


	return ret;
}

bool SceneMenu::Start()
{

	background = app->tex->Load("Assets/Textures/menu.png");
	menu = app->tex->Load("Assets/Textures/menu.png");
	playHover = app->tex->Load("Assets/Textures/menu_play_hover.png");
	continueHover = app->tex->Load("Assets/Textures/menu_continue_hover.png");
	settingsHover = app->tex->Load("Assets/Textures/menu_settings_hover.png");
	creditsHover = app->tex->Load("Assets/Textures/menu_credits_hover.png");
	exitHover = app->tex->Load("Assets/Textures/menu_exit_hover.png");
	playClick = app->tex->Load("Assets/Textures/menu_play_click.png");
	continueClick = app->tex->Load("Assets/Textures/menu_continue_click.png");
	settingsClick = app->tex->Load("Assets/Textures/menu_settings_click.png");
	creditsClick = app->tex->Load("Assets/Textures/menu_credits_click.png");
	exitClick = app->tex->Load("Assets/Textures/menu_exit_click.png");
	

	playButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Play", { 657, 315, 285, 64 }, this);
	continueButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Continue", { 657, 397, 285, 64 }, this);
	settingsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "Settings", { 657, 479, 285, 64 }, this);
	creditsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "Credits", { 657, 561, 285, 64 }, this);
	exitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "Exit", { 1517, 14, 63, 63 }, this);

	playButton->state = GuiControlState::NORMAL;
	continueButton->state = GuiControlState::NORMAL;
	settingsButton->state = GuiControlState::NORMAL;
	creditsButton->state = GuiControlState::NORMAL;
	exitButton->state = GuiControlState::NORMAL;

	return true;
}

bool SceneMenu::PreUpdate()
{
	return true;
}

bool SceneMenu::Update(float dt)
{
	if (playButton->state == GuiControlState::FOCUSED)
	{
		app->render->DrawTexture(playHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
	}
	else if (playButton->state == GuiControlState::PRESSED)
	{
		app->render->DrawTexture(playClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
		app->fade->FadeModules(this, (Module*)app->scene, 60.0f);
		app->guiManager->Disable();
		app->map->Enable();
		app->entityManager->Enable();
	}
	else if (continueButton->state == GuiControlState::FOCUSED)
	{
		app->render->DrawTexture(continueHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
	}
	else if (continueButton->state == GuiControlState::PRESSED)
	{
		app->render->DrawTexture(continueClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
	}
	else if (settingsButton->state == GuiControlState::FOCUSED)
	{
		app->render->DrawTexture(settingsHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
	}
	else if (settingsButton->state == GuiControlState::PRESSED)
	{
		app->render->DrawTexture(settingsClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
	}
	else if (creditsButton->state == GuiControlState::FOCUSED)
	{
		app->render->DrawTexture(creditsHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
	}
	else if (creditsButton->state == GuiControlState::PRESSED)
	{
		app->render->DrawTexture(creditsClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
	}
	else if (exitButton->state == GuiControlState::FOCUSED)
	{
		app->render->DrawTexture(exitHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
	}
	else if (exitButton->state == GuiControlState::PRESSED)
	{
		app->render->DrawTexture(exitClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
	}
	else
	{
		app->render->DrawTexture(menu, 0, 0, NULL, SDL_FLIP_NONE, 0);
	}

	return true;
}

bool SceneMenu::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

bool SceneMenu::CleanUp()
{
	LOG("Freeing SceneMenu");

	
	return true;
}
