#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include <irrKlang.h>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Enemy.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.

enum Level { START, HELP, CREDITS, LEVEL1, GAMEOVER };

class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();

private:
	void initShaders();
	void loadStaticImg(char* path);

private:
	Level level;
	Texture texture;
	Texture textureLife;
	Texture textureSpreadgun;
	Sprite *sprite;
	Sprite *spriteLife;
	Sprite *spriteSpreadgun;
	TileMap *map;
	Player *player;
	vector<shared_ptr<Enemy>> enemies;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	irrklang::ISound* backgroundMusic;

};


#endif // _SCENE_INCLUDE

