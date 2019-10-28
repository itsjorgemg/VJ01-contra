#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE

#include <memory>
#include "Sprite.h"
#include "TileMap.h"
#include "Bullet.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	void update(int deltaTime);
	void render();
	
	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);
	glm::ivec2 getPosition() const;
	glm::ivec2 getSize() const;
	glm::ivec2 getHitbox(bool top) const;
	glm::vec2 getDirection() const;
	vector<shared_ptr<Bullet>> getBullets() const;
	
private:
	bool bJumping;
	glm::ivec2 tileMapDispl, posPlayer;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	ShaderProgram* shaderProgram;
	vector<shared_ptr<Bullet>> bullets;

};


#endif // _PLAYER_INCLUDE


