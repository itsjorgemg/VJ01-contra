#include <cmath>
#include <iostream>
#include <algorithm>
#include <functional>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"
#include "Bullet.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4
#define RUN_SPEED 1
#define GUN_POSITION_X 5
#define GUN_POSITION_Y 10


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
};

void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram) {
	this->shaderProgram = &shaderProgram;
	bJumping = false;
	spritesheet.loadFromFile("images/main_character.png", TEXTURE_PIXEL_FORMAT_RGBA);
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

	sprite->changeAnimation(STAND_RIGHT);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

}

void Player::update(int deltaTime) {
	sprite->update(deltaTime);
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT)) {
		if (sprite->getCurrentAnimation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posPlayer.x -= RUN_SPEED;
		if (map->collisionMoveLeft(posPlayer + getHitbox(1), getHitbox(0))) {
			posPlayer.x += RUN_SPEED;
			sprite->changeAnimation(STAND_LEFT);
		}
	} else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) {
		if (sprite->getCurrentAnimation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		posPlayer.x += RUN_SPEED;
		if (map->collisionMoveRight(posPlayer + getHitbox(1), getHitbox(0))) {
			posPlayer.x -= RUN_SPEED;
			sprite->changeAnimation(STAND_RIGHT);
		}
	} else {
		if (sprite->getCurrentAnimation() == MOVE_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if (sprite->getCurrentAnimation() == MOVE_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}

	if (bJumping) {
		jumpAngle += JUMP_ANGLE_STEP;
		if (jumpAngle == 180) {
			bJumping = false;
			posPlayer.y = startY;
		} else {
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(glm::radians(float(jumpAngle))));
			if (jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer + getHitbox(1), getHitbox(0), &posPlayer.y);
		}
	} else {
		posPlayer.y += FALL_STEP;
		if (map->collisionMoveDown(posPlayer + getHitbox(1), getHitbox(0), &posPlayer.y)) {
			if (Game::instance().getSpecialKey(GLUT_KEY_UP)) {
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			} else if (Game::instance().getSpecialKey(GLUT_KEY_DOWN)) {
				posPlayer.y += FALL_STEP - 1;
				Game::instance().specialKeyReleased(GLUT_KEY_DOWN);
			} else if (Game::instance().getKey('\r')) {
				bullets.emplace_back(make_shared<Bullet>(posPlayer + getHitbox(1) + glm::ivec2(GUN_POSITION_X, GUN_POSITION_Y), getDirection(), *shaderProgram));
				Game::instance().getSoundEngine()->play2D("sounds/shoot.wav");
				Game::instance().keyReleased('\r');
			}
		}
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	for (auto bullet : bullets) {
		bullet->update(deltaTime);
	}
	bullets.erase(remove_if(bullets.begin(), bullets.end(), [](shared_ptr<Bullet> bullet) {
		return !bullet->isAlive();
		}), bullets.end());
}

void Player::render() {
	for (auto bullet : bullets) {
		bullet->render();
	}
	sprite->render();
}

void Player::setTileMap(TileMap* tileMap) {
	map = tileMap;
}

void Player::setPosition(const glm::vec2& pos) {
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

glm::ivec2 Player::getPosition() const {
	return posPlayer;
}

glm::ivec2 Player::getSize() const {
	return glm::ivec2(48, 48);
}

glm::ivec2 Player::getHitbox(bool top) const {
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
			if (top) return posPlayer;
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
			if (top) return posPlayer;
			else return glm::ivec2(48, 48);
			break;
		}
		break;
	default:
		if (top) return posPlayer;
		else return glm::ivec2(48, 48);
	}
}

glm::vec2 Player::getDirection() const {
	switch (sprite->getCurrentAnimation()) {
	case STAND_LEFT:
	case MOVE_LEFT:
		return glm::vec2(-1.0f, 0.0f);
	case STAND_RIGHT:
	case MOVE_RIGHT:
		return glm::vec2(1.0f, 0.0f);
	}
}

vector<shared_ptr<Bullet>> Player::getBullets() const {
	return bullets;
}
