#pragma once
#include "character.h"

struct BulletProp {
	int bullet_id = -1;
	int sprite_id = -1;
	int dmg = 1;
	int block_dmg = 1;
	float max_spd = 3;
	float init_spd = -1;
	float accel = 0;
};
static const BulletProp	bullet_array[] = {
		{	0,	0,	1,	1,	10,	-1,		0	},		// normal bullet
		{	1,	1,	10,	3,	20,	0.5f,	0.4f}		// rocket
};
enum BulletID {
	PISTOL,

};
class Bullet {
public:
	vec2 position = vec2(0);
	vec2 direction = vec2(0);
	float speed = 0;
	const BulletProp* prop;
	float life = 3;
	Bullet() {
	}
	Bullet(int bid, vec2 p, vec2 d) {
		prop = &bullet_array[bid];
		position = p;
		direction = d.normalize();
		speed = prop->init_spd < 0 ? prop->max_spd : prop->init_spd;
	}
	void update(float t) {
		if (prop->accel != 0) speed = std::min(speed + prop->accel, prop->max_spd);
		position += speed * direction * t;
		life = std::max(life - t, float(0));
	}
};
std::list<Bullet> bullet_instances;

struct GunProp {
	int gun_id;
	int bullet_id;
	int max_ammo = -1;
	float delay = 0;
	bool full_auto = false;
};
static const GunProp	gun_array[] = {
	{	0,	0,	-1,	0.08f,	false},	// pistol
	{	1,	0,	100,0.1f,	true},	// machine gun	
	{	2,	0,	30,	0.4f,	false},	// shot gun
	{	3,	1,	20, 0.6f,	false}	// rocket launcher
}; 
class Gun {
public:
	Character* crt;
	const GunProp* prop;
	float delay_t = 0;
	int ammo = 0;
	void swap_gun(int gid) {
		prop = &gun_array[gid];
		delay_t = 0;
		ammo = prop->max_ammo;
	}
	Gun() {

	}
	Gun(Character* c, int gid) {
		crt = c;
		swap_gun(gid);
	}
	void trigger(bool* b, float t) {
		if (*b && (ammo > 0 && delay_t <= 0 || prop->max_ammo < 0)) {
			fire();
			*b = *b && prop->full_auto;
		}
		delay_t = std::max(delay_t-t,float(0));
		if (prop->max_ammo >0 && ammo <= 0) swap_gun(0);
	}
	void fire() {
		delay_t = prop->delay;
		ammo = std::max(ammo-1,0);
		bullet_instances.push_back(Bullet(prop->bullet_id, crt->position, crt->direction));
		if (prop->gun_id == 2) {
			bullet_instances.push_back(Bullet(prop->bullet_id, crt->position, crt->direction + vec2(0,.3f)));
			bullet_instances.push_back(Bullet(prop->bullet_id, crt->position, crt->direction + vec2(0,-.3f)));
			bullet_instances.push_back(Bullet(prop->bullet_id, crt->position, crt->direction + vec2(0, .15f)));
			bullet_instances.push_back(Bullet(prop->bullet_id, crt->position, crt->direction + vec2(0, -.15f)));
		}
		else if (prop->gun_id == 3) {
			crt->velocity.x += crt->direction.normalize().x * -5;
		}
		printf("ammo left : %d\n", ammo);
	}
	void reload() {
		ammo = prop->max_ammo;
	}
};