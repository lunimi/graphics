#pragma once
static const uint MAP_WIDTH = 50;	// ∏  ≥–¿Ã
static const uint MAP_HEIGHT = 20;	// ∏  ≥Ù¿Ã

class block {
	
};
block	block_array[10];

std::vector<vertex> create_block_vertices()
{
	vec3 n_sample[] = {vec3(-1,-1,1), vec3(1,-1,1), vec3(-1,1,1), vec3(1,1,1), vec3(-1,-1,-1), vec3(1,-1,-1), vec3(-1,1,-1), vec3(1,1,-1) };
	std::vector<vertex> v = {};
	for (int i = 0; i < 8; i++) {
		v.push_back ({ (n_sample[i]+vec3(1,1,0))/2, n_sample[i].normalize(), vec2(0) });
	}
	return v;
}

struct Map {
	int map[MAP_WIDTH][MAP_HEIGHT] = {
		{1,1,1,1,1,1,1,1},
		{1,0,0,0},
		{1,0,0,0},
		{1,0,0,0},
		{1,0,0},
		{1,1,0},
		{1,1,1},
		{1,1,1,1,0,0,1},
		{1,1,1,1,0,0,1},
		{1,1,1,1,0,0,1},
		{1,1,0},
		{1,1,0},
		{1,1,0,0,0,0,0,1},
		{1,1,0,0,0,0,0,1},
		{1,1,0,0,0,0,0,1},
		{1,1,1,0,0,0,0,1},
		{1,1,1,1},
		{1,1,1,1,1},
		{1,1,0},
		{1,1,0,0,0,1},
		{1,1,0,0,0,1},
		{1,1,0,0,0,1},
		{1,1,1,1,1,1,1,1},
	};
	float gravity = 30;
	void create();
};
inline void Map::create()
{
}