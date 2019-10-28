#include <iostream>
#include <cmath>
#include <algorithm>
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

	irrklang::ISoundEngine* soundEngine = Game::instance().getSoundEngine();

	switch (level) {
	case START:
		loadStaticImg("images/startscreen.png");
		if (backgroundMusic != nullptr) {
			backgroundMusic->stop();
			backgroundMusic->drop();
		}
		backgroundMusic = soundEngine->play2D("sounds/intro.ogg", true, false, true);
		break;
	case HELP:
		loadStaticImg("images/helpscreen.png");
		break;
	case CREDITS:
		loadStaticImg("images/creditscreen.png");
		break;
	case LEVEL1:
		map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		player = new Player();
		player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
		player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
		player->setTileMap(map);

		vector<glm::vec2> enemiesPos = {glm::vec2(5, 1), glm::vec2(8, 3), glm::vec2(15, 1)};
		for (auto pos : enemiesPos) {
			enemies.emplace_back(make_shared<Enemy>());
			enemies[enemies.size() - 1]->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
			enemies[enemies.size() - 1]->setPosition(glm::vec2(pos.x * map->getTileSize(), pos.y * map->getTileSize() + enemies[enemies.size() - 1]->getSize().y / 2));
			enemies[enemies.size() - 1]->setTileMap(map);
		}

		projection = glm::ortho(0.0f, float(CAMERA_WIDTH), float(CAMERA_HEIGHT), 0.0f);
		if (backgroundMusic != nullptr) {
			backgroundMusic->stop();
			backgroundMusic->drop();
		}
		backgroundMusic = soundEngine->play2D("sounds/jungle-hangar.ogg", true, false, true);
		break;
	}
	
	currentTime = 0.0f;
}

void Scene::loadStaticImg(char* path) {
	texture.loadFromFile(path, TEXTURE_PIXEL_FORMAT_RGBA);
	texture.setMinFilter(GL_NEAREST);
	texture.setMagFilter(GL_NEAREST);
	sprite = Sprite::createSprite(glm::ivec2(STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT), glm::vec2(1.0f, 1.0f), &texture, &texProgram);
	projection = glm::ortho(0.0f, float(STARTSCREEN_WIDTH), float(STARTSCREEN_HEIGHT), 0.0f);
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;

	switch (level) {
	case START:
		if (Game::instance().getKey('\r')) {
			level = LEVEL1;
			init();
		} else if (Game::instance().getKey('h')) {
			level = HELP;
			init();
		} else if (Game::instance().getKey('c')) {
			level = CREDITS;
			init();
		}
		break;
	case HELP:
	case CREDITS:
		if (Game::instance().getKey('\r')) {
			Game::instance().keyReleased('\r');
			level = START;
			init();
		}
		break;
	case LEVEL1:
		player->update(deltaTime);
		int playerX = player->getPosition().x;
		for (auto enemy : enemies) {
			enemy->setLookingDirection(enemy->getPosition().x < playerX);
			enemy->update(deltaTime);
		}
		vector<shared_ptr<Enemy>> enemiesToRemove = vector<shared_ptr<Enemy>>();
		for (auto bullet : player->getBullets()) {
			glm::vec2 pos = bullet->getPosition();
			for (auto enemy : enemies) {
				glm::vec2 posE = enemy->getPosition() + enemy->getHitbox(1);
				glm::vec2 sizeE = enemy->getHitbox(0);
				if (pos.x > posE.x && pos.x < posE.x + sizeE.x &&
					pos.y > posE.y && pos.y < enemy->getPosition().y + sizeE.y) {
					enemiesToRemove.emplace_back(enemy);
					Game::instance().getSoundEngine()->play2D("sounds/enemyhit.wav");
				}
			}
		}
		for (auto enemyRemove : enemiesToRemove) {
			enemies.erase(remove(enemies.begin(), enemies.end(), enemyRemove), enemies.end());
		}

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
	case HELP:
	case CREDITS:
		sprite->render();
		break;
	case LEVEL1:
		map->render();
		player->render();
		for (auto enemy : enemies) {
			enemy->render();
		}
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



