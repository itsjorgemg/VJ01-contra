#include <iostream>
#include "Bullet.h"
#include "Game.h"

#define MAX_DISTANCE 100
#define SPEED 2

Bullet::Bullet(const glm::vec2& pos, const glm::vec2& dir, ShaderProgram& shaderProgram) :
	alive(true), position(pos), direction(dir) {
	spritesheet.loadFromFile("images/bullet.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(5, 5), glm::vec2(1.0f, 1.0f), &spritesheet, &shaderProgram);
	spritesheet.setMinFilter(GL_NEAREST);
	spritesheet.setMagFilter(GL_NEAREST);
	maxPosition = glm::vec2(pos.x, pos.y) + direction * float(MAX_DISTANCE);
}

void Bullet::update(int deltaTime) {
	sprite->update(deltaTime);

	setPosition(position + direction * float(SPEED));
	if (glm::distance(position, maxPosition) <= 0) {
		alive = false;
	}
}

void Bullet::render() {
	sprite->render();
}

void Bullet::setPosition(const glm::vec2& pos) {
	position = pos;
	sprite->setPosition(pos);
}

glm::vec2 Bullet::getPosition() const {
	return position;
}

bool Bullet::isAlive() const {
	return alive;
}

void Bullet::setAlive(bool a) {
	alive = a;
}
