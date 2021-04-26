#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec4 epos;
in vec4 wpos;
in vec3 position;
in vec3 norm;
in vec2 tc;

// the only output variable
out vec4 fragColor;

uniform mat4	view_matrix;
uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	// sunlight
uniform int		NUM_LIGHT;
uniform vec4	light_position2[10], Ia2[10], Id2[10], Is2[10];	//	lights
uniform vec4	Ka, Kd, Ks;

vec4 phong(vec3 l, vec3 n, vec3 h, vec4 Kd,vec4 ila, vec4 ild, vec4 ils)
{
	vec4 Ira = Ka * ila;									// ambient reflection
	vec4 Ird = max(Kd * dot(l, n) * ild, 0.0);					// diffuse reflection
	vec4 Irs = max(Ks * pow(dot(h, n), shininess) * ils, 0.0);	// specular reflection

	return Ira + Ird + Irs;
}


void main()
{
	vec4 lpos = view_matrix * light_position;

	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz - (lpos.a == 0.0 ? vec3(0) : p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l + v);	// the halfway vector

	//fragColor = phong(l, n, h, Kd,Ia,Id,Is);
	float len=10.f;
	for (int i = 0; i < NUM_LIGHT; i++) {
		lpos = view_matrix * light_position2[i];
		len = 0.5* abs(length(lpos-epos));
		len = len < 0.5f ? 0.5f : len;
		l = normalize(lpos.xyz - (lpos.a == 0.0 ? vec3(0) : p));
		h = normalize(l + v);
		fragColor += phong(l, n, h, Kd, Ia2[i], Id2[i], Is2[i]) / (len*len);
	}
	


}
