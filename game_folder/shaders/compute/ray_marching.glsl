#include<distance_estimators.glsl>

#define MAX_DIST 50
#define MIN_DIST 1e-6
#define MAX_MARCHES 150
#define NORMARCHES 2


void ray_march(inout vec4 pos, inout vec4 dir, inout vec4 var, float fov, float d0) 
{
	//March the ray
	for (; var.x < MAX_MARCHES; var.x += 1.0) {
		//if the distance from the surface is less than the distance per pixel we stop
		if(dir.w > MAX_DIST || pos.w<0)
		{
			break;
		}
		
		if(pos.w < max(fov*dir.w, MIN_DIST) && var.x > 0 && var.w < 1)
		{
			break;
		}
		
		dir.w += pos.w;
		pos.xyz += pos.w*dir.xyz;
		pos.w = DE(pos.xyz)-d0*dir.w;
		var.w = 0;
	}
	
	pos.w += d0*dir.w;
}

float sphere_intersection(vec3 r, vec3 p, vec4 sphere)
{
	p = p - sphere.xyz;
	if(p == vec3(0)) return sphere.w;
	
	float b = dot(p, r);
	float c = sphere.w*sphere.w - dot(p,p);
	float d = b*b + c;
	
	if((d <= 0) || (c <= 0)) //if no intersection
	{
		return 0;
	}
	else
	{
		return sqrt(d) - b; //use furthest solution in the direction of the ray
	}
}

float find_furthest_intersection(vec3 r, vec3 p, ivec2 id)
{
	float d = 0;
	ivec2 idR = min(id+1, group_size-1);
	ivec2 idL = max(id-1, 0);
	for(int i = idL.x; i <= idR.x; i++)
	{
		for(int j = idL.y; j <= idR.y; j++)
		{
			d = max(d, sphere_intersection(r,p,de_sph[i][j]));
		}
	}
	return d;
}


float find_furthest_intersection_all(vec3 r, vec3 p, ivec2 id)
{
	float d = 0;
	for(int i = 0; i < group_size; i++)
	{
		for(int j = 0; j < group_size; j++)
		{
			d = max(d, sphere_intersection(r,p,de_sph[i][j]));
		}
	}
	return d;
}

void normarch(inout vec4 pos)
{
	//calculate the normal
	vec4 norm = calcNormal(pos.xyz, pos.w/8); 
	norm.xyz = normalize(norm.xyz);
	pos.w = norm.w;
	
	//march in the direction of the normal
	#pragma unroll
	for(int i = 0; i < NORMARCHES; i++)
	{
		pos.xyz += pos.w*norm.xyz;
		pos.w = DE(pos.xyz);
	}
}
