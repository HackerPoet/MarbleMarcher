#include<distance_estimators.glsl>

#define MAX_DIST 50
#define MIN_DIST 0.0001
#define MAX_MARCHES 192

void ray_march(inout vec4 pos, inout vec4 dir, inout vec4 var, float fov) 
{
	//March the ray
	pos.w = DE(pos.xyz);
	for (; var.x < MAX_MARCHES; var.x += 1.0) {
		dir.w += pos.w;
		pos.xyz += pos.w*dir.xyz;
		pos.w = DE(pos.xyz);
		
		//if the distance from the surface is less than the distance per pixel we stop
		if(pos.w < max(fov*dir.w, MIN_DIST) || dir.w > MAX_DIST )
		{
			break;
		}
	}
}

//better to use a sampler though
vec4 interp(layout (rgba32f) image2D text, vec2 coord)
{
	//coord *= 0.99;
	ivec2 ci = ivec2(coord);
	vec2 d = round(coord - floor(coord));
	return imageLoad(text, ci)*(1-d.x)*(1-d.y) +
		   imageLoad(text, ci+ivec2(1,0))*d.x*(1-d.y) +
		   imageLoad(text, ci+ivec2(0,1))*(1-d.x)*d.y +
		   imageLoad(text, ci+ivec2(1))*d.x*d.y;
}

float sphere_intersection(vec3 r, vec3 p, vec4 sphere)
{
	p = sphere.xyz - p;
	if(p == vec3(0)) return sphere.w;
	
	r.x = dot(p, r);
	r.y = sphere.w*sphere.w - dot(r,r);
	r.z = r.x*r.x + r.y;
	
	if((r.z <= 0) || (r.y <= 0)) //if no intersection
	{
		return 0;
	}
	else
	{
		return sqrt(r.z) + r.x; //use furthest solution in the direction of the ray
	}
}

float find_furthest_intersection(vec3 r, vec3 p, ivec2 id)
{
	float d = 0;
	ivec2 idR = min(id+2, group_size);
	ivec2 idL = max(id-1, 0);
	for(int i = idL.x; i< idR.x; i++)
	{
		for(int j = idL.y; j < idR.y; j++)
		{
			d = max(d, sphere_intersection(r,p,de_sph[i][j]));
		}
	}
	return d;
}
/*
float divergence(in vec4 dir[bundle_length])
{
	return sqrt(1-pow(dot(dir[0], dir[bundle_length-1]),2));
}


void ray_bundle_marching1(inout vec4 pos[bundle_length], inout vec4 dir[bundle_length], inout vec4 var[bundle_length], int id)
{
	bool marching = true;
	float d = 0.f;
	
	//march central ray while safe to do so(ray bundle within ray cone)
	ray_march(pos[bundle_center], dir[bundle_center], var[bundle_center], divergence(dir));
	
	pos[bundle_center].w = 0;
	#pragma unroll
	for(int i = bundle_center+1, ii = i; i < bundle_length+bundle_center; ii = ++i%bundle_length)
	{
		pos[ii].xyz += dir[ii].xyz*dir[bundle_center].w;
		dir[ii].w = dir[bundle_center].w;
		var[ii] = var[bundle_center];
	}

	vec4 sphere = vec4(0);
	for(int m = 0; (m < MAX_MARCHES -var[bundle_center].x ) && marching; m++)
	{
		marching = false;
		sphere.w = 0;
		for(int i = 0; i < bundle_length; i++)
		{
			if((dir[i].w > MAX_DIST || (pos[i].w < fovray*dir[i].w &&  pos[i].w > 0)) && (dir[i].w > 0))
			{
				continue;
			}
			else
			{
				marching = true;
			}

			
			pos[i].xyz += dir[i].xyz*pos[i].w;
			dir[i].w += pos[i].w;
			var[i].x += 1.f;
			
			//memoryBarrierShared();
			d = sphere_intersection(dir[i].xyz, pos[i].xyz, sphere);
			
			if(d < fovray*dir[i].w)
			{
				pos[i].w = DE(pos[i].xyz);
				if(sphere.w < pos[i].w)
				{
					sphere = pos[i];
				}
			}
			else //if found a usable DE
			{
				pos[i].w = d;
			}
		}
	}
}*/