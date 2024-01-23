#ifndef __TORCH_H__
#define __TORCH_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Torch : public Entity
{
public:

	Torch();
	virtual ~Torch();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	bool isPicked = false;
	bool torchArea = false;
	bool torchIdleWaiting = false;
	float followTimer = 0.0f;
	float acceleration = 1.0f;
	int torchAudioFxId;

private:

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;

	Animation* currentAnim;
	Animation torchIdleAnim;
	Animation torchFireAnim;
};

#endif // __TORCH_H__