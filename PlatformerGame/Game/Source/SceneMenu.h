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

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

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


};

#endif // __SCENEMENU_H__