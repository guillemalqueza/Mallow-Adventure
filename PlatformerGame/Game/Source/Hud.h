#ifndef __HUD_H__
#define __HUD_H__

#include "Module.h"
#include "GuiControlButton.h"
#include "GuiManager.h"
#include "GuiControl.h"
#include "Timer.h"

struct SDL_Texture;

class Hud : public Module
{
public:

	Hud(bool enabled = true);

	virtual ~Hud();

	bool Awake(pugi::xml_node& config);

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void DrawTimer();

	bool playerDeadHud = false;
	bool spacePressed = false;
private:

	Timer timer;
	SDL_Texture* lifeBarTexture;
	SDL_Texture* deadScreenTexture;
	SDL_Texture* keyTexture;

	SDL_Rect keyRect = { 0,0,32,32 };

	SDL_Rect healthBar100 = { 0,0,238,80 };
	SDL_Rect healthBar80 = { 0,80,238,80 };
	SDL_Rect healthBar60 = { 0,160,238,80 };
	SDL_Rect healthBar40 = { 0,240,238,80 };
	SDL_Rect healthBar20 = { 0,320,238,80 };
	SDL_Rect healthBar0 = { 0,400,238,80 };


	SDL_Texture* exitNormal;
	SDL_Texture* exitHover;
	SDL_Texture* exitClick;

	SDL_Texture* returnNormal;
	SDL_Texture* returnHover;
	SDL_Texture* returnClick;

	SDL_Texture* pause;

	SDL_Texture* resumeNormal;
	SDL_Texture* resumeHover;
	SDL_Texture* resumeClick;

	SDL_Texture* settingsNormal;
	SDL_Texture* settingsHover;
	SDL_Texture* settingsClick;

	SDL_Texture* backToTitleNormal;
	SDL_Texture* backToTitleHover;
	SDL_Texture* backToTitleClick;


	GuiControlButton* exitButton;
	GuiControlButton* returnButton;
	GuiControlButton* resumeButton;
	GuiControlButton* settingsButton;
	GuiControlButton* backToTitleButton;
};

#endif // __HUD_H__