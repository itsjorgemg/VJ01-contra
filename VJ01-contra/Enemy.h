#ifndef _ENEMY_INCLUDE
#define _ENEMY_INCLUDE

#include <memory>
#include "Sprite.h"
#include "TileMap.h"
#include "Bullet.h"

class Enemy
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);
	void setLookingDirection(bool right);
	glm::ivec2 getPosition() const;
	glm::ivec2 getSize() const;
	glm::ivec2 getHitbox(bool top) const;
	glm::vec2 getDirection() const;

private:
	int shootBullet;
	glm::ivec2 tileMapDispl, position;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite* sprite;
	TileMap* map;
	ShaderProgram* shaderProgram;
	vector<shared_ptr<Bullet>> bullets;

};


#endif // _ENEMY_INCLUDE
