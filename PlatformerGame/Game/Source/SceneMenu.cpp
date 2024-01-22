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
	playNormal = app->tex->Load("Assets/Textures/Screens/Play/play_normal.png");
	playHover = app->tex->Load("Assets/Textures/Screens/Play/play_hover.png");
	playClick = app->tex->Load("Assets/Textures/Screens/Play/play_click.png");
	continueDisabled = app->tex->Load("Assets/Textures/Screens/Continue/continue_disabled.png");
	continueNormal = app->tex->Load("Assets/Textures/Screens/Continue/continue_normal.png");
	continueHover = app->tex->Load("Assets/Textures/Screens/Continue/continue_hover.png");
	continueClick = app->tex->Load("Assets/Textures/Screens/Continue/continue_click.png");
	settingsNormal = app->tex->Load("Assets/Textures/Screens/Settings/settings_normal.png");
	settingsHover = app->tex->Load("Assets/Textures/Screens/Settings/settings_hover.png");
	settingsClick = app->tex->Load("Assets/Textures/Screens/Settings/settings_click.png");
	creditsNormal = app->tex->Load("Assets/Textures/Screens/Credits/credits_normal.png");
	creditsHover = app->tex->Load("Assets/Textures/Screens/Credits/credits_hover.png");
	creditsClick = app->tex->Load("Assets/Textures/Screens/Credits/credits_click.png");
	exitNormal = app->tex->Load("Assets/Textures/Screens/More/exit_normal.png");
	exitHover = app->tex->Load("Assets/Textures/Screens/More/exit_hover.png");
	exitClick = app->tex->Load("Assets/Textures/Screens/More/exit_click.png");
	settings = app->tex->Load("Assets/Textures/Screens/More/settings.png");
	credits = app->tex->Load("Assets/Textures/Screens/More/credits.png");
	returnNormal = app->tex->Load("Assets/Textures/Screens/More/return_normal.png");
	returnHover = app->tex->Load("Assets/Textures/Screens/More/return_hover.png");
	returnClick = app->tex->Load("Assets/Textures/Screens/More/return_click.png");

	settingsBoxNormal = app->tex->Load("Assets/Textures/Screens/More/settings_box_normal.png");
	settingsBoxHover = app->tex->Load("Assets/Textures/Screens/More/settings_box_hover.png");

	settingsTick = app->tex->Load("Assets/Textures/Screens/tick.png");
	settingsSlider = app->tex->Load("Assets/Textures/Screens/slider.png");

	

	buttonFxHover = app->audio->LoadFx("Assets/Audio/Fx/button_hover.wav");
	buttonFxClick = app->audio->LoadFx("Assets/Audio/Fx/button_click.wav");
	
	//Menu Buttons
	playButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, NULL, playNormal, playHover, playClick, { 657, 315, 285, 64 }, this);
	continueButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, continueDisabled, continueNormal, continueHover, continueClick, { 657, 397, 285, 64 }, this);
	settingsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, NULL, settingsNormal, settingsHover, settingsClick, { 657, 479, 285, 64 }, this);
	creditsButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, NULL, creditsNormal, creditsHover, creditsClick, { 657, 561, 285, 64 }, this);
	exitButtonMenu = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, NULL, exitNormal, exitHover, exitClick, { 1517, 14, 63, 63 }, this);

	playButton->state = GuiControlState::NORMAL;
	continueButton->state = GuiControlState::DISABLED;
	settingsButton->state = GuiControlState::NORMAL;
	creditsButton->state = GuiControlState::NORMAL;
	exitButtonMenu->state = GuiControlState::NORMAL;

	//Settings Buttons
	settingsReturnButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, NULL, returnNormal, returnHover, returnClick, { 133, 92, 63, 63 }, this);
	settingsExitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, NULL, exitNormal, exitHover, exitClick, { 1419, 92, 63, 63 }, this);
	settingsFullScreenButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, NULL, settingsBoxNormal, settingsBoxHover, NULL, { 661, 494, 89, 89 }, this);
	settingsVSyncButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, NULL, settingsBoxNormal, settingsBoxHover, NULL, { 661, 613, 89, 89 }, this);

	settingsReturnButton->state = GuiControlState::NORMAL;
	settingsExitButton->state = GuiControlState::NORMAL;
	settingsFullScreenButton->state = GuiControlState::NORMAL;
	settingsVSyncButton->state = GuiControlState::NORMAL;

	//settingsMusicButton->state = GuiControlState::NORMAL;
	//settingsFxButton->state = GuiControlState::NORMAL;
	

	//Credits Buttons
	creditsReturnButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 10, NULL, returnNormal, returnHover, returnClick, { 133, 92, 63, 63 }, this);
	creditsExitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, NULL, exitNormal, exitHover, exitClick, { 1419, 92, 63, 63 }, this);

	return true;
}

bool SceneMenu::Update(float dt)
{
	//On menu screen
	if(onMenu && !onSettings && !onCredits){
		app->render->DrawTexture(background, 0, 0, NULL, SDL_FLIP_NONE, 0);
		if (playButton->state == GuiControlState::FOCUSED)
		{
			//app->render->DrawTexture(playHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (playButton->state == GuiControlState::PRESSED)
		{
			//app->render->DrawTexture(playClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
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
			//app->render->DrawTexture(continueHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (continueButton->state == GuiControlState::PRESSED)
		{
			//app->render->DrawTexture(continueClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
			
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
			}
		}
		else if (settingsButton->state == GuiControlState::FOCUSED)
		{
			//app->render->DrawTexture(settingsHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsButton->state == GuiControlState::PRESSED)
		{
			//app->render->DrawTexture(settingsClick, 0, 0, NULL, SDL_FLIP_NONE, 0);

			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
			}
			onSettings = true;
		}
		else if (creditsButton->state == GuiControlState::FOCUSED)
		{
			//app->render->DrawTexture(creditsHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (creditsButton->state == GuiControlState::PRESSED)
		{
			//app->render->DrawTexture(creditsClick, 0, 0, NULL, SDL_FLIP_NONE, 0);

			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				onCredits = true;
			}
		}
		else if (exitButtonMenu->state == GuiControlState::FOCUSED)
		{
			//app->render->DrawTexture(exitHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (exitButtonMenu->state == GuiControlState::PRESSED)
		{
			//app->render->DrawTexture(exitClick, 0, 0, NULL, SDL_FLIP_NONE, 0);

			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				SDL_Quit();
			}
		}
		else
		{
			//app->render->DrawTexture(menu, 0, 0, NULL, SDL_FLIP_NONE, 0);
			fxHoverPlayed = false;
			fxClickPlayed = false;
		}
	}
	//On settings screen
	else if (onSettings)
	{
		if (onMenu) {
			app->render->DrawTexture(background, 0, 0, NULL, SDL_FLIP_NONE, 0);
			app->render->DrawTexture(settings, 0, 0, NULL, SDL_FLIP_NONE, 0);	
		}
		if (settingsReturnButton->state == GuiControlState::FOCUSED)
		{
			//app->render->DrawTexture(settingsReturnHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsReturnButton->state == GuiControlState::PRESSED)
		{
			//app->render->DrawTexture(settingsReturnClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				onSettings = false;
			}
		}
		else if (settingsExitButton->state == GuiControlState::FOCUSED)
		{
			//app->render->DrawTexture(settingsExitHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsExitButton->state == GuiControlState::PRESSED)
		{
			//app->render->DrawTexture(settingsExitHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
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
			//app->render->DrawTexture(settingsFullScreenHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsFullScreenButton->state == GuiControlState::PRESSED)
		{
			//app->render->DrawTexture(settings, 0, 0, NULL, SDL_FLIP_NONE, 0);
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
			//app->render->DrawTexture(settingsVSyncHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsVSyncButton->state == GuiControlState::PRESSED)
		{
			//app->render->DrawTexture(settings, 0, 0, NULL, SDL_FLIP_NONE, 0);
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
			//app->render->DrawTexture(settings, 0, 0, NULL, SDL_FLIP_NONE, 0);
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
		app->render->DrawTexture(credits, 0, 0, NULL, SDL_FLIP_NONE, 0);
		if (creditsReturnButton->state == GuiControlState::FOCUSED)
		{
			//app->render->DrawTexture(creditsReturnHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (creditsReturnButton->state == GuiControlState::PRESSED)
		{
			//app->render->DrawTexture(creditsReturnClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				onCredits = false;
			}
		}
		else if (creditsExitButton->state == GuiControlState::FOCUSED)
		{
			//app->render->DrawTexture(creditsExitHover, 0, 0, NULL, SDL_FLIP_NONE, 0);
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (creditsExitButton->state == GuiControlState::PRESSED)
		{
			//app->render->DrawTexture(creditsExitClick, 0, 0, NULL, SDL_FLIP_NONE, 0);
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
			//app->render->DrawTexture(credits, 0, 0, NULL, SDL_FLIP_NONE, 0);
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
