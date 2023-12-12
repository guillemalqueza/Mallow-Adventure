#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Key.h"
#include "Jumper.h"
#include "CrumblingPlatform.h"
#include "LockDoor.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void CreateEntities(pugi::xml_node& config, const char* nodeName, EntityType entityType);

	void SetCameraPosition(int x, int y);

	void ClampCamera();

	void StartCameraShakeX(float duration, float intensity);

	void StartCameraShakeY(float duration, float intensity);

	void UpdateCameraShake();

	void StartLevel3();
	void StartLevel2();
	void StartLevel1();

	Player* player;	
	int cameraIdx = 0;
	bool cameraInitialized = true;
	bool level1Enabled = false;
	bool level2Enabled = false;
	bool level3Enabled = false;

private:
	SDL_Texture* backgroundTexture;
	SDL_Texture* backgroundTexture2;
	SDL_Rect bg;
	float textPosX, textPosY = 0;
	uint texW, texH;
	uint windowW, windowH;

	bool shakingCameraX = false;
	bool shakingCameraY = false;
	float shakeDuration;
	float shakeIntensity;
	float shakeTimer;

	float cameraSmoothingFactor = 0.04f;

	int levelWidth = 107 * 32;
	int levelHeight = 50 * 32;

	int level2Width = 140 * 32;
	int level2Height = 45 * 32;

	int playerX, playerY, cameraX, cameraY;
	bool changingLevel = false;
};

#endif // __SCENE_H__