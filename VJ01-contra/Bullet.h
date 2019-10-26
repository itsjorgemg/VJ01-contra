#ifndef _BULLET_INCLUDE
#define _BULLET_INCLUDE


#include "Sprite.h"

class Bullet
{

public:
	Bullet(const glm::vec2& pos, const glm::vec2& dir, ShaderProgram& shaderProgram);
	void init(ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();

	void setPosition(const glm::vec2& pos);
	glm::vec2 getPosition() const;
	bool isAlive() const;

private:
	glm::vec2 position;
	glm::vec2 maxPosition;
	glm::vec2 direction;
	Texture spritesheet;
	Sprite* sprite;
	bool alive;

};


#endif // _BULLET_INCLUDE


