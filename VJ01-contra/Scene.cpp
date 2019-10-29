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

#define SPRITELIFE_OFFSET 5

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
	case GAMEOVER:
		loadStaticImg("images/gameoverscreen.png");
		if (backgroundMusic != nullptr) {
			backgroundMusic->stop();
			backgroundMusic->drop();
		}
		backgroundMusic = soundEngine->play2D("sounds/gameover.ogg", false, false, true);
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

		textureLife.loadFromFile("images/life.png", TEXTURE_PIXEL_FORMAT_RGBA);
		textureLife.setMinFilter(GL_NEAREST);
		textureLife.setMagFilter(GL_NEAREST);
		spriteLife = Sprite::createSprite(glm::ivec2(8, 16), glm::vec2(1.0f, 1.0f), &textureLife, &texProgram);
		spriteLife->setPosition(glm::vec2(SPRITELIFE_OFFSET));

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
	case GAMEOVER:
		if (Game::instance().getKey('\r')) {
			Game::instance().keyReleased('\r');
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

		spriteLife->setPosition(glm::vec2(posPlayer + SPRITELIFE_OFFSET, SPRITELIFE_OFFSET));

		glm::vec2 posP = player->getPosition() + player->getHitbox(1);
		glm::vec2 sizeP = player->getHitbox(0);
		for (auto enemy : enemies) {
			enemy->setLookingDirection(enemy->getPosition().x < player->getPosition().x);
			enemy->update(deltaTime);

			for (auto bullet : enemy->getBullets()) {
				glm::vec2 pos = bullet->getPosition();
				if (pos.x > posP.x && pos.x < posP.x + sizeP.x &&
					pos.y > posP.y && pos.y < player->getPosition().y + sizeP.y) {
					player->decreaseLife();
					bullet->setAlive(false);
					Game::instance().getSoundEngine()->play2D("sounds/enemyhit.wav");
				}
			}
		}
		if (player->getLife() < 0) {
			level = GAMEOVER;
			init();
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
	case GAMEOVER:
		sprite->render();
		break;
	case LEVEL1:
		map->render();
		player->render();
		for (auto enemy : enemies) {
			enemy->render();
		}
		for (int i = 0; i < player->getLife(); i++) {
			if (i > 0) {
				spriteLife->setPosition(spriteLife->getPosition() + glm::vec2(16.0f, 0.0f));
			}
			spriteLife->render();
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



