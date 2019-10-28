#include <cmath>
#include <iostream>
#include <algorithm>
#include <functional>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Enemy.h"
#include "Game.h"
#include "Bullet.h"

#define MIN_SHOOT_INTERVAL 10
#define MAX_SHOOT_INTERVAL 80
#define GUN_POSITION_X 5
#define GUN_POSITION_Y 10

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
};

void Enemy::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram) {
	this->shaderProgram = &shaderProgram;
	shootBullet = rand() % (MAX_SHOOT_INTERVAL - MIN_SHOOT_INTERVAL + 1) + MIN_SHOOT_INTERVAL;
	spritesheet.loadFromFile("images/enemy_character.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(getSize(), glm::vec2(1.f / 10.f, 1.f / 10.f), &spritesheet, &shaderProgram);
	spritesheet.setMinFilter(GL_NEAREST);
	spritesheet.setMagFilter(GL_NEAREST);
	sprite->setNumberAnimations(4);

	sprite->setAnimationSpeed(STAND_LEFT, 8);
	sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));

	sprite->setAnimationSpeed(STAND_RIGHT, 8);
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(4.f / 10.f, 0.f));

	sprite->setAnimationSpeed(MOVE_LEFT, 8);
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f / 10.f, 1.f / 10.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(1.f / 10.f, 1.f / 10.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(2.f / 10.f, 1.f / 10.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(3.f / 10.f, 1.f / 10.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(4.f / 10.f, 1.f / 10.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(2.f / 10.f, 1.f / 10.f));

	sprite->setAnimationSpeed(MOVE_RIGHT, 8);
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(5.f / 10.f, 1.f / 10.f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(6.f / 10.f, 1.f / 10.f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(7.f / 10.f, 1.f / 10.f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(8.f / 10.f, 1.f / 10.f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(9.f / 10.f, 1.f / 10.f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(7.f / 10.f, 1.f / 10.f));

	sprite->changeAnimation(STAND_LEFT);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));

}

void Enemy::update(int deltaTime) {
	sprite->update(deltaTime);
	
	if (--shootBullet <= 0) {
		bullets.emplace_back(make_shared<Bullet>(position + getHitbox(1) + glm::ivec2(GUN_POSITION_X, GUN_POSITION_Y), getDirection(), *shaderProgram));
		shootBullet = rand() % (MAX_SHOOT_INTERVAL - MIN_SHOOT_INTERVAL + 1) + MIN_SHOOT_INTERVAL;
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
	for (auto bullet : bullets) {
		bullet->update(deltaTime);
	}
	bullets.erase(remove_if(bullets.begin(), bullets.end(), [](shared_ptr<Bullet> bullet) {
		return !bullet->isAlive();
	}), bullets.end());
}

void Enemy::render() {
	for (auto bullet : bullets) {
		bullet->render();
	}
	sprite->render();
}

void Enemy::setTileMap(TileMap* tileMap) {
	map = tileMap;
}

void Enemy::setPosition(const glm::vec2& pos) {
	position = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

glm::ivec2 Enemy::getPosition() const {
	return position;
}

glm::ivec2 Enemy::getSize() const {
	return glm::ivec2(48, 48);
}

glm::ivec2 Enemy::getHitbox(bool top) const {
	switch (sprite->getCurrentAnimation()) {
	case STAND_LEFT:
		if (top) return glm::ivec2(20, 13);
		else return glm::ivec2(14, 47);
		break;
	case STAND_RIGHT:
		if (top) return glm::ivec2(18, 13);
		else return glm::ivec2(13, 47);
		break;
	case MOVE_LEFT:
		switch (sprite->getCurrentKeyframe()) {
		case 0:
			if (top) return glm::ivec2(18, 13);
			else return glm::ivec2(15, 47);
			break;
		case 1:
			if (top) return glm::ivec2(18, 13);
			else return glm::ivec2(17, 47);
			break;
		case 2:
			if (top) return glm::ivec2(16, 16);
			else return glm::ivec2(20, 47);
			break;
		case 3:
			if (top) return glm::ivec2(16, 13);
			else return glm::ivec2(17, 47);
			break;
		case 4:
			if (top) return glm::ivec2(16, 13);
			else return glm::ivec2(17, 47);
			break;
		case 5:
			if (top) return glm::ivec2(16, 16);
			else return glm::ivec2(20, 47);
			break;
		default:
			if (top) return position;
			else return glm::ivec2(48, 48);
			break;
		}
		break;
	case MOVE_RIGHT:
		switch (sprite->getCurrentKeyframe()) {
		case 0:
			if (top) return glm::ivec2(19, 13);
			else return glm::ivec2(14, 47);
			break;
		case 1:
			if (top) return glm::ivec2(17, 13);
			else return glm::ivec2(15, 47);
			break;
		case 2:
			if (top) return glm::ivec2(16, 16);
			else return glm::ivec2(20, 47);
			break;
		case 3:
			if (top) return glm::ivec2(19, 14);
			else return glm::ivec2(15, 47);
			break;
		case 4:
			if (top) return glm::ivec2(17, 14);
			else return glm::ivec2(17, 47);
			break;
		case 5:
			if (top) return glm::ivec2(16, 16);
			else return glm::ivec2(20, 47);
			break;
		default:
			if (top) return position;
			else return glm::ivec2(48, 48);
			break;
		}
		break;
	default:
		if (top) return position;
		else return glm::ivec2(48, 48);
	}
}

glm::vec2 Enemy::getDirection() const {
	switch (sprite->getCurrentAnimation()) {
	case STAND_LEFT:
	case MOVE_LEFT:
		return glm::vec2(-1.0f, 0.0f);
	case STAND_RIGHT:
	case MOVE_RIGHT:
		return glm::vec2(1.0f, 0.0f);
	}
}