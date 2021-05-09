#pragma once


std::vector<vertex> create_sphere_vertices(uint N)
{
	std::vector<vertex> v;
	for (uint h = 0; h <= N; h++) {
		float t1 = PI * h / float(N), ch = cos(t1), sh = sin(t1);
		for (uint w = 0; w <= N; w++) {
			float t2 = 2.f * PI * w / float(N), cw = cos(t2), sw = sin(t2);
			v.push_back({ vec3(sh * cw,sh * sw,ch),vec3(-sh * cw,-sh * sw,-ch) ,vec2(t2 / (PI * 2.0f),1 - t1 / PI) });
		}
	}

	return v;
}