#include<distance_estimators.glsl>

#define MAX_DIST 50
#define MIN_DIST 0.0001
#define MAX_MARCHES 150
#define AMBIENT_MARCHES 5
#define NORMARCHES 3
#define BACKGROUND_COLOR vec3(0.4,0.7,1)

void ray_march(inout vec4 pos, inout vec4 dir, inout vec4 var, float fov, float d0) 
{
	//March the ray
	for (; var.x < MAX_MARCHES; var.x += 1.0) {
		//if the distance from the surface is less than the distance per pixel we stop
		if(dir.w > MAX_DIST)
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

//better to use a sampler though
vec4 interp(layout (rgba32f) image2D text, vec2 coord)
{
	//coord *= 0.99;
	ivec2 ci = ivec2(coord);
	vec2 d = coord - floor(coord);
	return imageLoad(text, ci)*(1-d.x)*(1-d.y) +
		   imageLoad(text, ci+ivec2(1,0))*d.x*(1-d.y) +
		   imageLoad(text, ci+ivec2(0,1))*(1-d.x)*d.y +
		   imageLoad(text, ci+ivec2(1))*d.x*d.y;
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
	vec4 norm = calcNormal(pos.xyz, pos.w/4); 
	pos.w = norm.w;
	
	//march in the direction of the normal
	#pragma unroll
	for(int i = 0; i < NORMARCHES; i++)
	{
		pos.xyz += pos.w*norm.xyz;
		pos.w = DE(pos.xyz);
	}
}

vec4 shading(in vec4 pos, in vec4 dir)
{
	//calculate the normal
	vec4 norm = calcNormal(pos.xyz, pos.w/4); 
	pos.w = norm.w;

	vec4 color = COL(pos.xyz - norm.w*norm.xyz);
	float occlusion_angle = 1;
	//march in the direction of the normal
	#pragma unroll
	for(int i = 0; i < AMBIENT_MARCHES; i++)
	{
		norm.w += pos.w;
		pos.xyz += pos.w*norm.xyz;
		pos.w = DE(pos.xyz);
		if(pos.w>0)
		{
			occlusion_angle = occlusion_angle*0.3 + 0.8*min(occlusion_angle, pos.w/norm.w);
		}
	}
	//its a surface area
	float ill_surface = pow(occlusion_angle,2);
	return color*BACKGROUND_COLOR*ill_surface;
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