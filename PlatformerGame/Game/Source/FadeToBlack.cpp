#include "FadeToBlack.h"

#include "App.h"
#include "Render.h"
#include "Window.h"
#include "Log.h"
#include "Scene.h"
#include "Map.h"
#include "SDL/include/SDL_render.h"

FadeToBlack::FadeToBlack() : Module()
{

	screenRect = {0, 0, 2000, 2000 };
	name.Create("fadetoblack");
}

FadeToBlack::~FadeToBlack()
{

}

bool FadeToBlack::Start()
{
	// Enable blending mode for transparency
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

bool FadeToBlack::Update(float dt)
{
	// Exit this function if we are not performing a fade
	if (currentStep == Fade_Step::NONE) return true;

	if (currentStep == Fade_Step::TO_BLACK)
	{
		++frameCount;
		if (frameCount >= maxFadeFrames)
		{
			if (modules)
			{
				moduleToDisable->Disable();
				moduleToEnable->Enable();
			}
			currentStep = Fade_Step::FROM_BLACK;
			modules = false;
		}
	}
	else
	{
		--frameCount;
		if (frameCount <= 0)
		{
			currentStep = Fade_Step::NONE;
			fadeFinished = true;
		}
		if (!activated)
		{
			activated = true;
			if (levelIdx == 1)
			{
				app->scene->isTorchActive = false;
				app->scene->StartLevel1();
			}
			else if (levelIdx == 2)
			{
				app->scene->newCameraIdx = 2;
				app->scene->isTorchActive = false;
				app->scene->StartLevel2();
			}
			else if (levelIdx == 3)
			{
				if (!app->scene->isTorchActive) app->scene->newCameraIdx = 3;
				app->scene->isTorchActive = false;
				app->scene->StartLevel3();
			}
		}
	}

	return true;
}

bool FadeToBlack::PostUpdate()
{
	// Exit this function if we are not performing a fade
	if (currentStep == Fade_Step::NONE) return true;

	float fadeRatio = (float)frameCount / (float)maxFadeFrames;

	// Render the black square with alpha on the screen
	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(fadeRatio * 255.0f));
	SDL_RenderFillRect(app->render->renderer, &screenRect);

	return true;
}

bool FadeToBlack::Fade(int levelIdx, float frames)
{
	bool ret = false;

	// If we are already in a fade process, ignore this call
	if(currentStep == Fade_Step::NONE)
	{
		currentStep = Fade_Step::TO_BLACK;
		frameCount = 0;
		maxFadeFrames = frames;
		this->levelIdx = levelIdx;
		ret = true;
	}
	activated = false;
	fadeFinished = false;

	return ret;
}

bool FadeToBlack::FadeModules(Module* moduleToDisable, Module* moduleToEnable, float frames)
{
	bool ret = false;
	modules = true;
	if (currentStep == Fade_Step::NONE)
	{
		currentStep = Fade_Step::TO_BLACK;
		frameCount = 0;
		maxFadeFrames = frames;

		this->moduleToDisable = moduleToDisable;
		this->moduleToEnable = moduleToEnable;
		ret = true;
	}
	return ret;
}