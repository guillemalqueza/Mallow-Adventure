#include "GuiControlSlider.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Log.h"
#include "GuiManager.h"

GuiControlSlider::GuiControlSlider(uint32 id, SDL_Rect bounds, SDL_Texture* textureDisabled, SDL_Texture* textureNormal, SDL_Texture* textureFocused, SDL_Texture* texturePressed, SDL_Rect sliderBounds) : GuiControl(GuiControlType::BUTTON, id)
{
    this->bounds = bounds;
    this->text = text;
    this->textureDisabled = textureDisabled;
    this->textureNormal = textureNormal;
    this->textureFocused = textureFocused;
    this->texturePressed = texturePressed;
    this->sliderBounds = sliderBounds;

    canClick = true;
    drawBasic = false;

    sliderPosition = bounds.x;
}

GuiControlSlider::~GuiControlSlider()
{

}

bool GuiControlSlider::Update(float dt)
{
    if (state != GuiControlState::HIDDEN)
    {
        app->input->GetMousePosition(mouseX, mouseY);

       
        if (mouseX > sliderBounds.x && mouseX < sliderBounds.x + sliderBounds.w - bounds.w && mouseY > bounds.y && mouseY < bounds.y + bounds.h) {
            state = GuiControlState::FOCUSED;

            if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
                state = GuiControlState::PRESSED;
                initialMouseX = mouseX;
            }

            if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
                state = GuiControlState::PRESSED;
                bounds.x = mouseX;
            }

            if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
                NotifyObserver();
            }
        }
        else {
            state = GuiControlState::NORMAL;
        }

        if (app->guiManager->debugGui)
        {
            app->render->DrawRectangle({ bounds.x - 6, bounds.y - 3, bounds.w + 6, bounds.h + 6 }, 0, 0, 255, 100);
        }

        switch (state)
        {
        case GuiControlState::DISABLED:
            app->render->DrawTexture(textureDisabled, bounds.x, bounds.y, NULL, SDL_FLIP_NONE, 0);
            break;
        case GuiControlState::NORMAL:
            app->render->DrawTexture(textureNormal, bounds.x, bounds.y, NULL, SDL_FLIP_NONE, 0);
            break;
        case GuiControlState::FOCUSED:
            app->render->DrawTexture(textureFocused, bounds.x, bounds.y, NULL, SDL_FLIP_NONE, 0);
            break;
        case GuiControlState::PRESSED:
            app->render->DrawTexture(textureFocused, bounds.x, bounds.y, NULL, SDL_FLIP_NONE, 0);
            break;
        }
    }

    return false;
}
