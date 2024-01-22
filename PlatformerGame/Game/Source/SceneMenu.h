#ifndef __SCENEMENU_H__
#define __SCENEMENU_H__

#include "Module.h"
#include "GuiControlButton.h"
#include "GuiManager.h"
#include "GuiControl.h"

struct SDL_Texture;

class SceneMenu : public Module
{
public:

	SceneMenu(bool enabled = true);

	virtual ~SceneMenu();

	bool Awake(pugi::xml_node& config);

	bool Start();

	bool Update(float dt);

	bool CleanUp();


private:

	SDL_Texture* background;
	SDL_Texture* menu;
	SDL_Texture* playHover;
	SDL_Texture* playClick;
	SDL_Texture* continueHover;
	SDL_Texture* continueClick;
	SDL_Texture* settingsHover;
	SDL_Texture* settingsClick;	
	SDL_Texture* creditsHover;
	SDL_Texture* creditsClick;
	SDL_Texture* exitHover;
	SDL_Texture* exitClick;

	GuiControlButton* playButton;
	GuiControlButton* continueButton;
	GuiControlButton* settingsButton;
	GuiControlButton* creditsButton;
	GuiControlButton* exitButton;

	SDL_Texture* settings;
	SDL_Texture* settingsReturnHover;
	SDL_Texture* settingsReturnClick;
	SDL_Texture* settingsExitHover;
	SDL_Texture* settingsExitClick;
	SDL_Texture* settingsTick;
	SDL_Texture* settingsSlider;
	SDL_Texture* settingsFullScreenHover;
	SDL_Texture* settingsFullScreenClick;
	SDL_Texture* settingsVSyncHover;
	SDL_Texture* settingsVSyncClick;

	SDL_Texture* credits;
	SDL_Texture* creditsReturnHover;
	SDL_Texture* creditsReturnClick;
	SDL_Texture* creditsExitHover;
	SDL_Texture* creditsExitClick;

	GuiControlButton* settingsReturnButton;
	GuiControlButton* settingsExitButton;
	GuiControlButton* settingsFullScreenButton;
	GuiControlButton* settingsVSyncButton;
	GuiControlButton* settingsMusicButton;
	GuiControlButton* settingsFxButton;

	GuiControlButton* creditsReturnButton;
	GuiControlButton* creditsExitButton;

	int buttonFxHover;
	int buttonFxClick;
	bool fxHoverPlayed = false;
	bool fxClickPlayed = false;
	
	bool onMenu = true;
	bool onSettings = false;
	bool onCredits = false;
	bool fullScreen = false;
	bool vSync = false;

};

#endif // __SCENEMENU_H__