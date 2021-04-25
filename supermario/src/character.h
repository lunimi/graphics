#pragma once
#include "map.h"
#include <cmath>

class Character {
public:
	Map*	map;
	vec2	position;
	float	max_speed = 5.0f;
	float	max_jump = 3.5f;
	Character(Map* map0, vec2 pos) {
		map = map0;
		position = pos;
	}
	void	update(float t, bool b);
	void	move_right();
	void	move_left();
	void	jump();
private:
	vec2	velocity = vec2(0);
	vec2	acceration = vec2(0);
	bool	is_jump = true;
};

inline void Character::update(float t, bool b)
{
	velocity += acceration * t;
	int px = int(position.x);
	int py = int(position.y);
	position += velocity * t + acceration * powf(t, 2);
	int x = int(position.x);
	int y = int(position.y);	

	if (!is_jump) {
		if (map->map[px][py - 1] == 0 && map->map[px+1][py - 1] == 0) {
			is_jump = true;
		}
		acceration.y = 0;
		velocity.y = 0;
		if (!b) acceration.x = -velocity.x * 10;
		else acceration.x = 0;
	}
	else {
		acceration.x *= 0.8f;
		acceration.y = -map->gravity;
	}

	printf("%f %f\n", velocity.x, velocity.y);
	if (is_jump) {
		printf("jump\n");
	}
	else {
		printf("nojump\n");
	}

	if (velocity.x > 0 && map->map[x + 1][py] == 1)
	{
		position.x = float(px+0.999f);
		velocity.x = 0;
	}
	else if (velocity.x < 0 && map->map[x][py] == 1)
	{
		position.x = float(px);
		velocity.x = 0;
	}
	for (int i = py; i >= y; i--) {
		if (map->map[px][i] == 1 || map->map[px+1][i] == 1)
		{
			position.y = float(i + 1);
			is_jump = false;
			break;
		}
	}
	for (int i = py; i <= y; i++) {
		if (map->map[px][i+1] == 1 || map->map[px + 1][i+1] == 1)
		{
			position.y = float(i);
			velocity.y = 0;
			map->map[px][i + 1] = 0;
			break;
		}
	}
}


inline void Character::move_right()
{
	if (is_jump) {
		if (velocity.x < max_speed/2)
		{
			acceration.x = max_speed*5;
		}
	}
	else
	{
		velocity.x = max_speed;
	}
}

inline void Character::move_left()
{
	if (is_jump ) {
		if (velocity.x > -max_speed/2)
		{
			acceration.x = -max_speed*5;
		}
	}
	else
	velocity.x = -max_speed;
}

inline void Character::jump()
{
	if (!is_jump) {
		float jv = sqrt(2 * max_jump * map->gravity);
		velocity.y = jv;
		is_jump = true;
	}
}