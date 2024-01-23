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

	settingsTick = app->tex->Load("Assets/Textures/Screens/More/tick.png");
	settingsSlider = app->tex->Load("Assets/Textures/Screens/More/slider.png");

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
	settingsFullScreenButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 8, NULL, settingsBoxNormal, settingsBoxHover, settingsTick, { 661, 494, 89, 89 }, this);
	settingsVSyncButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 9, NULL, settingsBoxNormal, settingsBoxHover, settingsTick, { 661, 613, 89, 89 }, this);
	settingsMusicButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 10, NULL, settingsSlider, settingsSlider, settingsSlider, { 1124, 259, 30, 80 }, this, { 1000, 259, 300, 80 });
	settingsFxButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 11, NULL, settingsSlider, settingsSlider, settingsSlider, { 1124, 376, 30, 80 }, this, { 1000, 376, 300, 80 });

	settingsReturnButton->state = GuiControlState::NORMAL;
	settingsExitButton->state = GuiControlState::NORMAL;
	settingsFullScreenButton->state = GuiControlState::NORMAL;
	settingsVSyncButton->state = GuiControlState::NORMAL;

	settingsMusicButton->state = GuiControlState::NORMAL;
	settingsFxButton->state = GuiControlState::NORMAL;
	

	//Credits Buttons
	creditsReturnButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 10, NULL, returnNormal, returnHover, returnClick, { 133, 92, 63, 63 }, this);
	creditsExitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, NULL, exitNormal, exitHover, exitClick, { 1419, 92, 63, 63 }, this);

	return true;
}

bool SceneMenu::Update(float dt)
{
	//On menu screen
	if(onMenu && !onSettings && !onCredits){

		settingsReturnButton->state = GuiControlState::HIDDEN;
		settingsExitButton->state = GuiControlState::HIDDEN;
		settingsFullScreenButton->state = GuiControlState::HIDDEN;
		settingsVSyncButton->state = GuiControlState::HIDDEN;

		creditsExitButton->state = GuiControlState::HIDDEN;
		creditsReturnButton->state = GuiControlState::HIDDEN;

		app->render->DrawTexture(background, 0, 0, NULL, SDL_FLIP_NONE, 0);
		if (playButton->state == GuiControlState::FOCUSED)
		{
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (playButton->state == GuiControlState::PRESSED)
		{
			app->fade->FadeModules(this, (Module*)app->scene, 60.0f);
			app->map->Enable();
			app->entityManager->Enable();
			app->guiManager->Disable();

			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
			}
		}
		else if (continueButton->state == GuiControlState::FOCUSED)
		{
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (continueButton->state == GuiControlState::PRESSED)
		{
			
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
			}
		}
		else if (settingsButton->state == GuiControlState::FOCUSED)
		{
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsButton->state == GuiControlState::PRESSED)
		{
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
			}
			onSettings = true;
			settingsExitButton->state = GuiControlState::NORMAL;
			settingsReturnButton->state = GuiControlState::NORMAL;
			settingsFullScreenButton->state = GuiControlState::NORMAL;
			settingsVSyncButton->state = GuiControlState::NORMAL;
		}
		else if (creditsButton->state == GuiControlState::FOCUSED)
		{
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (creditsButton->state == GuiControlState::PRESSED)
		{

			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				onCredits = true;
				creditsExitButton->state = GuiControlState::NORMAL;
				creditsReturnButton->state = GuiControlState::NORMAL;

			}
		}
		else if (exitButtonMenu->state == GuiControlState::FOCUSED)
		{
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (exitButtonMenu->state == GuiControlState::PRESSED)
		{

			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				SDL_Quit();
			}
		}
		else
		{
			fxHoverPlayed = false;
			fxClickPlayed = false;
		}
	}
	//On settings screen
	else if (onSettings)
	{
		playButton->state = GuiControlState::HIDDEN;
		continueButton->state = GuiControlState::HIDDEN;
		settingsButton->state = GuiControlState::HIDDEN;
		creditsButton->state = GuiControlState::HIDDEN;
		exitButtonMenu->state = GuiControlState::HIDDEN;

		if (onMenu) {
			app->render->DrawTexture(background, 0, 0, NULL, SDL_FLIP_NONE, 0);
			app->render->DrawTexture(settings, 0, 0, NULL, SDL_FLIP_NONE, 0);	
		}
		if (settingsReturnButton->state == GuiControlState::FOCUSED)
		{
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsReturnButton->state == GuiControlState::PRESSED)
		{
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				onSettings = false;
				playButton->state = GuiControlState::NORMAL;
				continueButton->state = GuiControlState::DISABLED;
				settingsButton->state = GuiControlState::NORMAL;
				creditsButton->state = GuiControlState::NORMAL;
				exitButtonMenu->state = GuiControlState::NORMAL;
			}
		}
		else if (settingsExitButton->state == GuiControlState::FOCUSED)
		{
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsExitButton->state == GuiControlState::PRESSED)
		{
			
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
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsFullScreenButton->state == GuiControlState::PRESSED)
		{
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
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (settingsVSyncButton->state == GuiControlState::PRESSED)
		{
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
			fxHoverPlayed = false;
			fxClickPlayed = false;
		}
		//FullScreen and VSync
		if (fullScreen)	SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN);
		else SDL_SetWindowFullscreen(app->win->window, 0);

		if (vSync)	app->render->vsync = true;
		else	app->render->vsync = false;
	}
	//On credits screen
	else if (onCredits)
	{
		playButton->state = GuiControlState::HIDDEN;
		continueButton->state = GuiControlState::HIDDEN;
		settingsButton->state = GuiControlState::HIDDEN;
		creditsButton->state = GuiControlState::HIDDEN;
		exitButtonMenu->state = GuiControlState::HIDDEN;

		app->render->DrawTexture(background, 0, 0, NULL, SDL_FLIP_NONE, 0);
		app->render->DrawTexture(credits, 0, 0, NULL, SDL_FLIP_NONE, 0);
		if (creditsReturnButton->state == GuiControlState::FOCUSED)
		{
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (creditsReturnButton->state == GuiControlState::PRESSED)
		{
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
				onCredits = false;
				playButton->state = GuiControlState::NORMAL;
				continueButton->state = GuiControlState::NORMAL;
				settingsButton->state = GuiControlState::NORMAL;
				creditsButton->state = GuiControlState::NORMAL;
				exitButtonMenu->state = GuiControlState::NORMAL;
			}
		}
		else if (creditsExitButton->state == GuiControlState::FOCUSED)
		{
			if (fxHoverPlayed == false)
			{
				app->audio->PlayFx(buttonFxHover);
				fxHoverPlayed = true;
			}
		}
		else if (creditsExitButton->state == GuiControlState::PRESSED)
		{
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
