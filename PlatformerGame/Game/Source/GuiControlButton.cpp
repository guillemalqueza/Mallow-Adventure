#include "GuiControlButton.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"

GuiControlButton::GuiControlButton(uint32 id, SDL_Rect bounds, SDL_Texture* textureDisabled, SDL_Texture* textureNormal, SDL_Texture* textureFocused, SDL_Texture* texturePressed) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;
	this->textureDisabled = textureDisabled;
	this->textureNormal = textureNormal;
	this->textureFocused = textureFocused;
	this->texturePressed = texturePressed;

	canClick = true;
	drawBasic = false;
}

GuiControlButton::~GuiControlButton()
{

}

bool GuiControlButton::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{
		// L15: DONE 3: Update the state of the GUiButton according to the mouse position
		app->input->GetMousePosition(mouseX, mouseY);

		//If the position of the mouse if inside the bounds of the button 
		if (mouseX > bounds.x && mouseX < bounds.x + bounds.w && mouseY > bounds.y && mouseY < bounds.y + bounds.h) {
		
			state = GuiControlState::FOCUSED;

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				state = GuiControlState::PRESSED;
			}
			
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
			}
		}
		else {
			state = GuiControlState::NORMAL;
		}

		//L15: DONE 4: Draw the button according the GuiControl State
		switch (state)
		{
		case GuiControlState::DISABLED:
			app->render->DrawTexture(textureDisabled, bounds.x, bounds.y, &bounds, SDL_FLIP_NONE, 0);
			break;
		case GuiControlState::NORMAL:
			app->render->DrawTexture(textureNormal, bounds.x, bounds.y, NULL, SDL_FLIP_NONE, 0);
			break;
		case GuiControlState::FOCUSED:
			app->render->DrawTexture(textureFocused, bounds.x, bounds.y, NULL, SDL_FLIP_NONE, 0);
			break;
		case GuiControlState::PRESSED:
			app->render->DrawTexture(texturePressed, bounds.x, bounds.y, &bounds, SDL_FLIP_NONE, 0);
			break;
		}

		//app->render->DrawText(text.GetString(), bounds.x, bounds.y, bounds.w, bounds.h);

	}

	return false;
}

