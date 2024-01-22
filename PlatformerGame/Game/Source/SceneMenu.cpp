#include "SceneMenu.h"

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
	background = app->tex->Load("Assets/Textures/Screens/background.png");
	menu = app->tex->Load("Assets/Textures/Screens/menu.png");
	playHover = app->tex->Load("Assets/Textures/Screens/menu_play_hover.png");
	continueHover = app->tex->Load("Assets/Textures/Screens/menu_continue_hover.png");
	settingsHover = app->tex->Load("Assets/Textures/Screens/menu_settings_hover.png");
	creditsHover = app->tex->Load("Assets/Textures/Screens/menu_credits_hover.png");
	exitHover = app->tex->Load("Assets/Textures/Screens/menu_exit_hover.png");
	playClick = app->tex->Load("Assets/Textures/Screens/menu_play_click.png");
	continueClick = app->tex->Load("Assets/Textures/Screens/menu_continue_click.png");
	settingsClick = app->tex->Load("Assets/Textures/Screens/menu_settings_click.png");
	creditsClick = app->tex->Load("Assets/Textures/Screens/menu_credits_click.png");
	exitClick = app->tex->Load("Assets/Textures/Screens/menu_exit_click.png");

	settings = app->tex->Load("Assets/Textures/Screens/settings.png");
	settingsReturnHover = app->tex->Load("Assets/Textures/Screens/settings_return_hover.png");
	settingsReturnClick = app->tex->Load("Assets/Textures/Screens/settings_return_click.png");
	settingsExitHover = app->tex->Load("Assets/Textures/Screens/settings_exit_hover.png");
	settingsExitClick = app->tex->Load("Assets/Textures/Screens/settings_exit_click.png");
	settingsTick = app->tex->Load("Assets/Textures/Screens/tick.png");
	settingsSlider = app->tex->Load("Assets/Textures/Screens/slider.png");
	settingsFullScreenHover = app->tex->Load("Assets/Textures/Screens/settings_fullscreen_hover.png");
	settingsVSyncHover = app->tex->Load("Assets/Textures/Screens/settings_vsync_hover.png");

	credits = app->tex->Load("Assets/Textures/Screens/credits.png");
	creditsReturnHover = app->tex->Load("Assets/Textures/Screens/credits_return_hover.png");
	creditsReturnClick = app->tex->Load("Assets/Textures/Screens/credits_return_click.png");
	creditsExitHover = app->tex->Load("Assets/Textures/Screens/credits_exit_hover.png");
	creditsExitClick = app->tex->Load("Assets/Textures/Screens/credits_exit_click.png");

	buttonFxHover = app->audio->LoadFx("Assets/Audio/Fx/button_hover.wav");
	buttonFxClick = app->audio->LoadFx("Assets/Audio/Fx/button_click.wav");
	
	playButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Play", { 657, 315, 285, 64 }, this);
	continueButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Continue", { 657, 397, 285, 64 }, this);
	settingsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "Settings", { 657, 479, 285, 64 }, this);
	creditsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "Credits", { 657, 561, 285, 64 }, this);
	exitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "Exit", { 1517, 14, 63, 63 }, this);

	playButton->state = GuiControlState::NORMAL;
	continueButton->state = GuiControlState::DISABLED;
	settingsButton->state = GuiControlState::NORMAL;
	creditsButton->state = GuiControlState::NORMAL;
	exitButton->state = GuiControlState::NORMAL;

	settingsReturnButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "R", { 133, 92, 63, 63 }, this);
	settingsExitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "E", { 1419, 92, 63, 63 }, this);
	settingsFullScreenButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "FullScreen", { 661, 494, 89, 89 }, this);
	settingsVSyncButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "VSync", { 661, 613, 89, 89 }, this);
	//settingsMusicButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 10, "Music", { 1124, 259, 25, 78 }, this);
	//settingsFxButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 11, "Fx", { 1007, 376, 25, 78 }, this);


	settingsReturnButton->state = GuiControlState::NORMAL;
	settingsExitButton->state = GuiControlState::NORMAL;
	settingsFullScreenButton->state = GuiControlState::NORMAL;
	settingsVSyncButton->state = GuiControlState::NORMAL;
	//settingsMusicButton->state = GuiControlState::NORMAL;
	//settingsFxButton->state = GuiControlState::NORMAL;
	
	creditsReturnButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 12, "R", { 133, 92, 63, 63 }, this);
	creditsExitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 13, "E", { 1419, 92, 63, 63 }, this);

	return true;
}

bool SceneMenu::Update(float dt)
{
	//On menu screen
	if(onMenu && !onSettings && !onCredits){
		app->render->DrawTexture(background, 0, 0, NULL, SDL_FLIP_NONE, 0);
		if (playButton->state == GuiControlState::FOCUSED)
		{
			app->render->DrawTexture(playHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (playButton->state == GuiControlState::PRESSED)
		{
			app->render->DrawTexture(playClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
			app->fade->FadeModules(this, (Module*)app->scene, 60.0f);
			app->guiManager->Disable();
			app->map->Enable();
			app->entityManager->Enable();
			app->particleManager->Enable();

			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
			}
		}
		else if (continueButton->state == GuiControlState::FOCUSED)
		{
			app->render->DrawTexture(continueHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (continueButton->state == GuiControlState::PRESSED)
		{
			app->render->DrawTexture(continueClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
			
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
			}
		}
		else if (settingsButton->state == GuiControlState::FOCUSED)
		{
			app->render->DrawTexture(settingsHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsButton->state == GuiControlState::PRESSED)
		{
			app->render->DrawTexture(settingsClick, 0, 0, NULL, SDL_FLIP_NONE, 0);

			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
			}
			onSettings = true;
		}
		else if (creditsButton->state == GuiControlState::FOCUSED)
		{
			app->render->DrawTexture(creditsHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (creditsButton->state == GuiControlState::PRESSED)
		{
			app->render->DrawTexture(creditsClick, 0, 0, NULL, SDL_FLIP_NONE, 0);

			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				onCredits = true;
			}
		}
		else if (exitButton->state == GuiControlState::FOCUSED)
		{
			app->render->DrawTexture(exitHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (exitButton->state == GuiControlState::PRESSED)
		{
			app->render->DrawTexture(exitClick, 0, 0, NULL, SDL_FLIP_NONE, 0);

			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				SDL_Quit();
			}
		}
		else
		{
			app->render->DrawTexture(menu, 0, 0, NULL, SDL_FLIP_NONE, 0);
			fxHoverPlayed = false;
			fxClickPlayed = false;
		}
	}
	//On settings screen
	else if (onSettings)
	{
		if (onMenu) app->render->DrawTexture(background, 0, 0, NULL, SDL_FLIP_NONE, 0);
		if (settingsReturnButton->state == GuiControlState::FOCUSED)
		{
			app->render->DrawTexture(settingsReturnHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsReturnButton->state == GuiControlState::PRESSED)
		{
			app->render->DrawTexture(settingsReturnClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				onSettings = false;
			}
		}
		else if (settingsExitButton->state == GuiControlState::FOCUSED)
		{
			app->render->DrawTexture(settingsExitHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsExitButton->state == GuiControlState::PRESSED)
		{
			app->render->DrawTexture(settingsExitHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				onSettings = false;
				SDL_Quit();
			}
		}
		else if (settingsFullScreenButton->state == GuiControlState::FOCUSED)
		{
			app->render->DrawTexture(settingsFullScreenHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsFullScreenButton->state == GuiControlState::PRESSED)
		{
			app->render->DrawTexture(settings, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				if (fullScreen == false) fullScreen = true;
				else fullScreen = false;
			}
		}
		else if (settingsVSyncButton->state == GuiControlState::FOCUSED)
		{
			app->render->DrawTexture(settingsVSyncHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsVSyncButton->state == GuiControlState::PRESSED)
		{
			app->render->DrawTexture(settings, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				if (vSync == false) vSync = true;
				else vSync = false;
			}
		}
		else
		{
			app->render->DrawTexture(settings, 0, 0, NULL, SDL_FLIP_NONE, 0);
			fxHoverPlayed = false;
			fxClickPlayed = false;
		}
		//FullScreen and VSync
		if (fullScreen)
		{
			app->render->DrawTexture(settingsTick, 666, 473, NULL, SDL_FLIP_NONE, 0);
			SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN);
		}
		else
		{
			SDL_SetWindowFullscreen(app->win->window, 0);
		}
		if (vSync){
			app->render->DrawTexture(settingsTick, 666, 589, NULL, SDL_FLIP_NONE, 0);
			app->render->vsync = true;
		}
		else
		{
			app->render->vsync = false;
		}
	}
	//On credits screen
	else if (onCredits)
	{
		app->render->DrawTexture(background, 0, 0, NULL, SDL_FLIP_NONE, 0);

		if (creditsReturnButton->state == GuiControlState::FOCUSED)
		{
			app->render->DrawTexture(creditsReturnHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (creditsReturnButton->state == GuiControlState::PRESSED)
		{
			app->render->DrawTexture(creditsReturnClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				onCredits = false;
			}
		}
		else if (creditsExitButton->state == GuiControlState::FOCUSED)
		{
			app->render->DrawTexture(creditsExitHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (creditsExitButton->state == GuiControlState::PRESSED)
		{
			app->render->DrawTexture(creditsExitClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				onCredits = false;
				SDL_Quit();
			}
		}	
		else
		{
			app->render->DrawTexture(credits, 0, 0, NULL, SDL_FLIP_NONE, 0);
			fxHoverPlayed = false;
			fxClickPlayed = false;
		}
	}



	return true;
}

bool SceneMenu::CleanUp()
{
	LOG("Freeing SceneMenu");

	
	return true;
}
