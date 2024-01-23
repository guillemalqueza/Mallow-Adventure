#include "GuiControlSlider.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Log.h"

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

        if (mouseX > bounds.x && mouseX < bounds.x + bounds.w && mouseY > bounds.y && mouseY < bounds.y + bounds.h) {
            state = GuiControlState::FOCUSED;

            if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
                state = GuiControlState::PRESSED;
                initialMouseX = mouseX;
            }

            if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
                state = GuiControlState::PRESSED;
                int delta = mouseX - initialMouseX;

                int newSliderX = bounds.x + delta;
                if (newSliderX < sliderBounds.x) {
                    newSliderX = sliderBounds.x;
                }
                else if (newSliderX > sliderBounds.x + sliderBounds.w - bounds.w) {
                    newSliderX = sliderBounds.x + sliderBounds.w - bounds.w;
                }

                bounds.x = newSliderX;
                initialMouseX = mouseX;
            }

            if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
                NotifyObserver();
            }
        }
        else {
            state = GuiControlState::NORMAL;
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
