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
#include "Hud.h"

#include "Defs.h"
#include "Log.h"

SceneMenu::SceneMenu(bool enabled) : Module(enabled)
{
	name.Create("scenemenu");
}

SceneMenu::~SceneMenu()
{}

pugi::xml_node configNode2;

bool SceneMenu::Awake(pugi::xml_node& config)
{
	LOG("Loading SceneMenu");
	bool ret = true;

	configNode2 = config;

	return ret;
}

bool SceneMenu::Start()
{
	//Load textures
	background = app->tex->Load(configNode2.child("background").attribute("texturepath").as_string());
	playNormal = app->tex->Load(configNode2.child("playNormal").attribute("texturepath").as_string());
	playHover = app->tex->Load(configNode2.child("playHover").attribute("texturepath").as_string());
	playClick = app->tex->Load(configNode2.child("playClick").attribute("texturepath").as_string());
	continueDisabled = app->tex->Load(configNode2.child("continueDisabled").attribute("texturepath").as_string());
	continueNormal = app->tex->Load(configNode2.child("continueNormal").attribute("texturepath").as_string());
	continueHover = app->tex->Load(configNode2.child("continueHover").attribute("texturepath").as_string());
	continueClick = app->tex->Load(configNode2.child("continueClick").attribute("texturepath").as_string());
	settingsNormal = app->tex->Load(configNode2.child("settingsNormal").attribute("texturepath").as_string());
	settingsHover = app->tex->Load(configNode2.child("settingsHover").attribute("texturepath").as_string());
	settingsClick = app->tex->Load(configNode2.child("settingsClick").attribute("texturepath").as_string());
	creditsNormal = app->tex->Load(configNode2.child("creditsNormal").attribute("texturepath").as_string());
	creditsHover = app->tex->Load(configNode2.child("creditsHover").attribute("texturepath").as_string());
	creditsClick = app->tex->Load(configNode2.child("creditsClick").attribute("texturepath").as_string());
	exitNormal = app->tex->Load(configNode2.child("exitNormal").attribute("texturepath").as_string());
	exitHover = app->tex->Load(configNode2.child("exitHover").attribute("texturepath").as_string());
	exitClick = app->tex->Load(configNode2.child("exitClick").attribute("texturepath").as_string());
	settings = app->tex->Load(configNode2.child("settings").attribute("texturepath").as_string());
	credits = app->tex->Load(configNode2.child("credits").attribute("texturepath").as_string());
	returnNormal = app->tex->Load(configNode2.child("returnNormal").attribute("texturepath").as_string());
	returnHover = app->tex->Load(configNode2.child("returnHover").attribute("texturepath").as_string());
	returnClick = app->tex->Load(configNode2.child("returnClick").attribute("texturepath").as_string());
	settingsBoxNormal = app->tex->Load(configNode2.child("settingsBoxNormal").attribute("texturepath").as_string());
	settingsBoxHover = app->tex->Load(configNode2.child("settingsBoxHover").attribute("texturepath").as_string());
	settingsTick = app->tex->Load(configNode2.child("settingsTick").attribute("texturepath").as_string());
	settingsSlider = app->tex->Load(configNode2.child("settingsSlider").attribute("texturepath").as_string());
	buttonFxHover = app->audio->LoadFx(configNode2.child("buttonFxHover").attribute("path").as_string());
	buttonFxClick = app->audio->LoadFx(configNode2.child("buttonFxClick").attribute("path").as_string());

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
	settingsMusicButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 10, NULL, settingsSlider, settingsSlider, settingsSlider, { 1124, 259, 30, 80 }, this, { 663, 259, 613, 80 });
	settingsFxButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 11, NULL, settingsSlider, settingsSlider, settingsSlider, { 1124, 376, 30, 80 }, this, { 663, 376, 613, 80 });

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

		//Hide settings buttons
		settingsReturnButton->state = GuiControlState::HIDDEN;
		settingsExitButton->state = GuiControlState::HIDDEN;
		settingsFullScreenButton->state = GuiControlState::HIDDEN;
		settingsVSyncButton->state = GuiControlState::HIDDEN;
		settingsMusicButton->state = GuiControlState::HIDDEN;
		settingsFxButton->state = GuiControlState::HIDDEN;

		creditsExitButton->state = GuiControlState::HIDDEN;
		creditsReturnButton->state = GuiControlState::HIDDEN;

		//Render background 
		app->render->DrawTexture(background, 0, 0, NULL, SDL_FLIP_NONE, 0);
		//Check if buttons are focused or pressed. If pressed, do the action. With sound effects.
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
			if (fxClickPlayed == false)
			{
				app->audio->PlayFx(buttonFxClick);
				fxClickPlayed = true;
			}
			app->fade->FadeModules(this, (Module*)app->scene, 60.0f);
			app->map->Enable();
			app->entityManager->Enable();
			app->hud->Enable();
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
			//Show settings buttons
			onSettings = true;
			settingsExitButton->state = GuiControlState::NORMAL;
			settingsReturnButton->state = GuiControlState::NORMAL;
			settingsFullScreenButton->state = GuiControlState::NORMAL;
			settingsVSyncButton->state = GuiControlState::NORMAL;
			settingsMusicButton->state = GuiControlState::NORMAL;
			settingsFxButton->state = GuiControlState::NORMAL;
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
				return false;
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
		//Hide menu buttons
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
				//Show menu buttons
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
				return false;
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
		//Hide menu buttons
		playButton->state = GuiControlState::HIDDEN;
		continueButton->state = GuiControlState::HIDDEN;
		settingsButton->state = GuiControlState::HIDDEN;
		creditsButton->state = GuiControlState::HIDDEN;
		exitButtonMenu->state = GuiControlState::HIDDEN;

		//Render background and credits
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
				//Show menu buttons
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
				return false;
			}
		}	
		else
		{
			//Reset sound effects
			fxHoverPlayed = false;
			fxClickPlayed = false;
		}
	}



	return true;
}

bool SceneMenu::CleanUp()
{
	LOG("Freeing SceneMenu");

	app->tex->UnLoad(background);
	app->tex->UnLoad(playNormal);
	app->tex->UnLoad(playHover);
	app->tex->UnLoad(playClick);
	app->tex->UnLoad(continueDisabled);
	app->tex->UnLoad(continueNormal);
	app->tex->UnLoad(continueHover);
	app->tex->UnLoad(continueClick);
	app->tex->UnLoad(settingsNormal);
	app->tex->UnLoad(settingsHover);
	app->tex->UnLoad(settingsClick);
	app->tex->UnLoad(creditsNormal);
	app->tex->UnLoad(creditsHover);
	app->tex->UnLoad(creditsClick);
	app->tex->UnLoad(exitNormal);
	app->tex->UnLoad(exitHover);
	app->tex->UnLoad(exitClick);
	app->tex->UnLoad(settings);
	app->tex->UnLoad(returnNormal);
	app->tex->UnLoad(returnHover);
	app->tex->UnLoad(returnClick);
	app->tex->UnLoad(settingsBoxNormal);
	app->tex->UnLoad(settingsBoxHover);
	app->tex->UnLoad(settingsTick);
	app->tex->UnLoad(settingsSlider);
	app->tex->UnLoad(credits);

	
	return true;
}
