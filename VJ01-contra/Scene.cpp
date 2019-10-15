#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 0
#define SCREEN_Y 0

#define INIT_PLAYER_X_TILES 1
#define INIT_PLAYER_Y_TILES 0

#define CAMERA_WIDTH 32 * 8
#define CAMERA_HEIGHT 32 * 7

#define STARTSCREEN_WIDTH 240
#define STARTSCREEN_HEIGHT 240

Scene::Scene()
{
	level = START;
	map = NULL;
	player = NULL;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
}


void Scene::init()
{
	initShaders();

	switch (level) {
	case START:
		texture.loadFromFile("images/startscreen.png", TEXTURE_PIXEL_FORMAT_RGBA);
		texture.setMinFilter(GL_NEAREST);
		texture.setMagFilter(GL_NEAREST);
		sprite = Sprite::createSprite(glm::ivec2(STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT), glm::vec2(1.0f, 1.0f), &texture, &texProgram);
		projection = glm::ortho(0.0f, float(STARTSCREEN_WIDTH), float(STARTSCREEN_HEIGHT), 0.0f);
		break;
	case LEVEL1:
		map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		player = new Player();
		player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
		player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
		player->setTileMap(map);
		projection = glm::ortho(0.0f, float(CAMERA_WIDTH), float(CAMERA_HEIGHT), 0.0f);
		break;
	}
	
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;

	switch (level) {
	case START:
		if (Game::instance().getKey('\r')) {
			level = LEVEL1;
			init();
		}
		break;
	case LEVEL1:
		player->update(deltaTime);

		float posPlayer = player->getPosition().x + player->getSize().x / 2 - CAMERA_WIDTH / 2;
		float rightLimit = (map->getSize().x * map->getTileSize()) - CAMERA_WIDTH;
		posPlayer = glm::clamp(posPlayer, 0.0f, rightLimit);
		projection = glm::ortho(posPlayer, float(CAMERA_WIDTH) + posPlayer, float(CAMERA_HEIGHT), 0.0f);
	break;
	}
}

void Scene::render()
{
	glm::mat4 modelview;

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	switch (level) {
	case START:
		sprite->render();
		break;
	case LEVEL1:
		map->render();
		player->render();
	break;
	}
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}



