#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec4 epos;
in vec3 position;
in vec3 norm;
in vec2 tc;

// the only output variable
out vec4 fragColor;

uniform int		mode;
uniform int		block_id;
uniform sampler2D	TEX_skybox;
uniform sampler2D	TEX_title;
uniform sampler2D	TEX_blocks;
uniform sampler2D	TEX_blocks_opacity;
//light
uniform mat4	view_matrix;
uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	//sunlight
uniform int		NUM_LIGHT;
uniform vec4	light_position2[10], Ia2[10], Id2[10], Is2[10];	//lights
uniform vec4	Ka, Kd, Ks;

vec4 phong(vec3 l, vec3 n, vec3 h, vec4 Kd, vec4 ila, vec4 ild, vec4 ils)
{
	
	vec4 Ira,Ird,Irs;
	if (block_id == 4) {
		Ira = 0.5f*Ka * ila;											// ambient reflection
		Ird = 0.7f*max(Kd * dot(l, n) * ild, 0.0);					// diffuse reflection
		Irs = max(Ks * pow(dot(h, n), shininess*5.f) * ils, 0.0);	// specular reflection
	}
	else {
		Ira = Ka * ila;
		Ird = max(Kd * dot(l, n) * ild, 0.0);
		Irs = max(Ks * pow(dot(h, n), shininess) * ils, 0.0);
	}
		
	return Ira + Ird + Irs;
}

void main()
{
	switch (mode) {
	case 0://normal
		vec4 lpos = view_matrix * light_position;

		vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
		vec3 p = epos.xyz;			// 3D position of this fragment
		vec3 l = normalize(lpos.xyz - (lpos.a == 0.0 ? vec3(0) : p));	// lpos.a==0 means directional light
		vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
		vec3 h = normalize(l + v);	// the halfway vector
		//sunlight effect
		fragColor = phong(l, n, h, Kd,Ia,Id,Is);
		
		//lights effect
		float len = 10.f;
		for (int i = 0; i < NUM_LIGHT; i++) {
			lpos = view_matrix * light_position2[i];
			len = 0.4f*abs(length(lpos - epos));
			len = len < 0.3f ? 0.3f : len;
			l = normalize(lpos.xyz - (lpos.a == 0.0 ? vec3(0) : p));
			h = normalize(l + v);
			fragColor +=  phong(l, n, h, Kd, Ia2[i], Id2[i], Is2[i]) / (len *len);
		}
		fragColor *= texture(TEX_blocks, tc+vec2(0,0.1f* (block_id-1)));//vec4(normalize(norm), 1.0);
		//if(block_id==4)	fragColor.a *= 0.15f;
		fragColor.a *= texture(TEX_blocks_opacity, tc + vec2(0, 0.1f * (block_id - 1))).x;
		break;
	case 1:
		//skybox
		fragColor = texture(TEX_skybox, tc);
		break;
	case 2:
		//title
		fragColor = texture(TEX_title, tc);
		break;
	default:
		fragColor = vec4(normalize(norm), 1.0);
		break;
	}

	
}
