#pragma once
#include "map.h"
#include <cmath>

class Hitbox {
public:
	vec4	box_rec = { -0.3f, -0.5f, 0.3f, 0.5f };	// rectangle margin
	Hitbox() {
	}
	Hitbox(float l, float b, float r, float t) {
		box_rec = vec4(l, b, r, t);
	}
	bool collid(vec2 pos) {
		bool left_check = box_rec.x < pos.x&& box_rec.y < pos.y;
		bool right_check = pos.x < box_rec.z&& pos.y < box_rec.a;
		return  left_check && right_check;
	}
	float width() {
		return box_rec.z - box_rec.x;
	}
	float height() {
		return box_rec.a - box_rec.y;
	}
};
class Character {
public:
	Character() {
	}
	Character(Map* p, vec2 pos) {
		mapp = p;
		position = pos;
	}
	bool	alive = true;		// ����

	float	invinc_time = 1.5f;	// �ǰ� �� ���� �ð�
	float	max_speed = 5.0f;	// �ִ� �̵��ӵ�
	float	speed = 1.0f;		// �̵� ���ӵ�
	float	max_jump = 3.5f;	// �ִ� ����
	float	mass = 1; 			// �ݵ� ����
	int	max_hp = 30;			// �ִ� ü��
	Map* mapp;					// �� ������

	vec2	direction = vec2(1,0);	// ������ �ϴ� ����
	Hitbox	hitbox = Hitbox();	// ��Ʈ �ڽ�
	vec2	position;			// ��ġ	
	vec2	velocity = vec2(0);
	vec2	acceleration = vec2(0);
	int		hp = max_hp;		// ���� ü��
	float	invinc_t = 0;		// ���� Ÿ�̸�
	bool	is_jump = true;		// ���� ����
	Block*	stand_blockp;		// �ٷ� �Ʒ� ��� ������

	// function
	bool	hit(int damage, vec2 hit_pos);
	bool	heal(int amount);
	void	move_right();
	void	move_left();
	void	jump();
	bool	checkJump();
	bool	checkDeath();
	void	physics(float t, bool moving);
	void	update(float t, bool moving);

};

inline bool Character::hit(int damage, vec2 hit_pos) {
	if (invinc_t <= 0) {
		printf("took %d damage! hp: %d\n", damage, hp);
		hp -= damage;
		if (length(hit_pos) > 0)
		{
			velocity = hit_pos.normalize() * 10 / mass;
		}
		if (velocity.y > 0) {
			is_jump = true;
		}
		invinc_t = invinc_time;
		alive = !checkDeath();
		return true;
	}
	return false;
}
inline bool Character::heal(int amount) {
	bool flag = hp <= max_hp;
	hp = std::min(hp + amount, max_hp);
	return flag;
}
inline bool Character::checkDeath() {
	return hp <= 0 || position.y < -5;
}

inline bool Character::checkJump() {
	vec2 stand_block_pos = position - vec2(0, 1);
	//printf("check jump : %f %f \n",stand_block_pos.x,position.y);
	stand_blockp = mapp->block(stand_block_pos);
	if (stand_blockp == 0) return 0;
	return stand_blockp->prop->block_id == 0;
}
inline void Character::physics(float t, bool moving)
{
	vec2 temp_pos = position;
	if (is_jump) {
		acceleration.x *= 0.8f;
		acceleration.y = -mapp->gravity;
		stand_blockp = 0;
	}
	else {
		is_jump = checkJump();
		acceleration.y = 0;
		velocity.y = 0;
		if (stand_blockp->prop->collid_dmg > 0) {
			is_jump = true;
			hit(stand_blockp->prop->collid_dmg, vec2(0, 1));
		}
		if (!moving) acceleration.x = -velocity.x * 10;
		else acceleration.x = 0;
	}
	velocity += acceleration * t;
	position += velocity * t + acceleration * powf(t, 2);
	if (position.x - temp_pos.x >= 0) { //ĳ���Ͱ� ���������� ������
		int temp_pos_x = int(position.x + hitbox.box_rec.z);
		for (int i = int(temp_pos.x + hitbox.box_rec.z); i <= temp_pos_x; i++)
		{
			Block* bp = mapp->block(vec2(float(i), temp_pos.y));
			if (bp == 0) continue;
			if (bp->prop->block_id > 0) {
				position.x = i - hitbox.box_rec.z;
				if (bp->prop->collid_dmg > 0) {
					hit(bp->prop->collid_dmg, vec2(-1, 0));
				}
				break;
			}
		}
	}
	if (position.x - temp_pos.x <= 0) { //ĳ���Ͱ� �������� ������
		int temp_pos_x = int(position.x + hitbox.box_rec.x);
		for (int i = int(temp_pos.x + hitbox.box_rec.x); i >= temp_pos_x; i--)
		{
			Block* bp = mapp->block(vec2(float(i), temp_pos.y));
			if (bp == 0) continue;
			if (bp->prop->block_id > 0) {
				position.x = i + 1 - hitbox.box_rec.x;
				if (bp->prop->collid_dmg > 0) {
					hit(bp->prop->collid_dmg, vec2(1, 0));
				}
				break;
			}
		}
	}
	if(is_jump)
	{
		if (position.y - temp_pos.y > 0) { //ĳ���Ͱ� �������� ������
			int temp_pos_y = int(position.y + hitbox.box_rec.a);
			for (int i = int(temp_pos.y + hitbox.box_rec.a); i <= temp_pos_y; i++)
			{
				Block* bp = mapp->block(vec2(position.x, float(i)));
				if (bp == 0) continue;
				if (bp->prop->max_hp > 0) {
					bp->hit(1);
				}
				if (bp->prop->block_id > 0) {
					position.y = i - hitbox.box_rec.a;
					velocity.y = 0; // õ�� �΋H��
					if (bp->prop->collid_dmg > 0) {
						hit(bp->prop->collid_dmg, vec2(0, -1));
					}
					break;
				}
			}
		}
		else {
			int temp_pos_y = int(position.y + hitbox.box_rec.y);
			for (int i = int(temp_pos.y + hitbox.box_rec.y); i >= temp_pos_y; i--)
			{
				Block* bp = mapp->block(vec2(position.x, float(i)));
				if (bp == 0) continue;
				if (bp->prop->block_id > 0) {
					position.y = i + 1 - hitbox.box_rec.y;
					if (bp->prop->collid_dmg > 0) {
						is_jump = hit(bp->prop->collid_dmg, vec2(0, 1));
					}
					else {
						is_jump = false;
					}
					break;
				}
			}
		}
	}
}

inline void Character::update(float t, bool moving)
{
	invinc_t = std::max(invinc_t - t, 0.0f);
	physics(t, moving);
}


inline void Character::move_right()
{
	direction.x = 1;
	if (is_jump) {
		if (velocity.x < max_speed/2)
		{
			acceleration.x = max_speed*5;
		}
	}
	else
	{
		velocity.x = std::min(max_speed, velocity.x + speed);
	}
}

inline void Character::move_left()
{
	direction.x = -1;
	if (is_jump ) {
		if (velocity.x > -max_speed/2)
		{
			acceleration.x = -max_speed*5;
		}
	}
	else
		velocity.x = std::max(-max_speed, velocity.x - speed);
}

inline void Character::jump()
{
	if (!is_jump) {
		float jv = sqrt(2 * max_jump * mapp->gravity);
		velocity.y = jv;
		is_jump = true;
	}
}

class Enemy : public Character
{
public:
	Character* crt;
	float	active_range = 7;	// �ν� �ݰ� (x��)
	float	action_timer = 0;	// �ൿ ��� �ð�
	int		damage = 1;
	bool	active = false;
	~Enemy() {
		printf("Enemy died\n");
	}
	Enemy(Map* mp, Character* cp, vec2 pos) {
		mass = 5;
		invinc_time = 0;	// �ǰ� �� ���� �ð�
		max_speed = 4.5f;	// �ִ� �̵��ӵ�
		speed = 0.3f;
		max_jump = 1.5f;	// �ִ� ����
		mapp = mp;
		max_hp = 10;
		hp = max_hp;
		position = pos;
		crt = cp;
	}
	void update(float t, bool moving);
	void move();
	bool check_active();
};
inline bool Enemy::check_active() {
	return active || hp < max_hp || length(crt->position - position) < active_range;
}
inline void Enemy::update(float t, bool moving) {
	physics(t, moving);
	if (active) {
		move();
	}
	else {
		active = check_active();
	}
}
inline void Enemy::move() {
	if (crt->position.x > position.x) {
		move_right();
	}
	else {
		move_left();
	}
	if (crt->position.y > position.y) {
		jump();
	}
}
