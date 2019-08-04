#include<distance_estimators.glsl>

#define MAX_DIST 50
#define MIN_DIST 0.01
float FOVperPixel = 0.01;

void ray_march(inout vec4 pos, inout vec3 dir) 
{
	//March the ray
	pos.w = DE(pos.xyz);
	float td = 0;
	for (float s = 0; s < MAX_MARCHES; s += 1.0) {
		//if the distance from the surface is less than the distance per pixel we stop
		float min_dist = max(FOVperPixel*td, MIN_DIST);
		if(td > MAX_DIST || pos.w < min_dist)
		{
			break;
		}
		td += pos.w;
		pos.xyz += pos.w*dir;
		pos.w = DE(pos.xyz);
	}
}


float sphere_intersection(vec3 r, vec3 p, vec4 sphere)
{
	vec3 dp = sphere.xyz - p;
	float b = dot(dp, r);
	float c = sphere.w*sphere.w - dot(dp,dp);
	float D = b*b + c;
	if((D < 0) || (c < 0)) //if no intersection
	{
		return 0;
	}
	else
	{
		return sqrt(D) + b; //use furthest solution in the direction of the ray
	}
}
/*


bool march_ray_bundle(ivec2 p)
{
	int i = p.x, j = p.y;
	//wait untill all data is written
	memoryBarrierShared();
	
	//if the distance from the surface is less than the distance per pixel we stop
	if (DE_data[i][j] != 0 && DE_data[i][j] < ray_array[i][j].fov*ray_array[i][j].td)
	{
		return true;
	} 
	
	float new_march = find_furthest_d(ray_array[i][j].dir, get_p(ray_array[i][j]), p);
	
	//wait untill all data is written, not sure if we need 2 barriers though
	memoryBarrierShared();
	
	//check if the ray is inside one of the local DE spheres
	if(new_march > 0)
	{
		ray_array[i][j].td += new_march;
		DE_data[i][j] = 0;
	}
	else //if not -> calculate a DE at this point
	{
		DE_data[i][j] = DE(vec4(get_p(ray_array[i][j]),1));
	}
	return false;
}*/

vec4 find_furthest_DE_sphere(vec3 r, vec3 p)
{
	float d = 0;
	vec4 desphere;
	//check through all of the local bundles
	for(int i = 0; i < block_size; i++)
	{
		float this_d = sphere_intersection(r, p, ray_pos[i]);
		if(this_d > d)
		{
			d = this_d;
			desphere = ray_pos[i];
		}
	}
	return desphere;
}

float find_furthest_DE_local(vec3 r, vec3 p, int id)
{
	float d = 0;
	ivec2 id2 = getGpos(id);
	ivec2 id11 = max(id2 - 1,0);
	ivec2 id22 = min(id2 + 1,group_size);
	//check through all of the local bundles
	for(int i = id11.x; i < id22.x; i++)
	{
		for(int j = id11.y; j < id22.y; j++)
		{
			float this_d = sphere_intersection(r, p, ray_pos[i + group_size*j]);
			d = max(this_d, d);
		}
	}
	return d;
}

float find_furthest_DE(vec3 r, vec3 p, int id)
{
	float d = 0;
	//check through all of the local bundles
	/*
	for(int i = 0; i < block_size; i++)
	{
		memoryBarrierShared();
		float this_d = sphere_intersection(r, p, ray_pos[i]);
		d = max(this_d, d);
	}
	*/
	d = max(sphere_intersection(r, p, ray_pos[max(id-1, 0)]), d);
	d = max(sphere_intersection(r, p, ray_pos[id]), d);
	return d;
}

void ray_bundle_marching(inout vec4 pos[bundle_length], inout vec3 dir[bundle_length], int id)
{
	float bundle_fov = bundle_size*fovray;
	float td = 0;
	float d = 0;
	bool marching = true;
	
	for(int m = 0; (m < MAX_MARCHES) && marching; m++)
	{
		memoryBarrierShared();
		ray_pos[id].w = DE(ray_pos[id].xyz);
		memoryBarrierShared();
		if(ray_pos[id].w < bundle_fov*td) 
		{
			break;
			marching = false;
			//march the ray bundle
			for(int i = 0; i < bundle_length; i++)
			{
				if(pos[i].w > fovray)
				{
					marching = true;
					pos[i].w = find_furthest_DE_local(dir[i], pos[i].xyz, id);
					//if found a usable DE
					if(pos[i].w  > 0)
					{
						pos[i].xyz += dir[i]*pos[i].w;
					}
					else //calculate a new DE
					{
						pos[i].w = DE(pos[i].xyz);
						pos[i].xyz += dir[i]*pos[i].w;
					}
				}
			}
		}
		else
		{
			d = find_furthest_DE(ray_dir[id], ray_pos[id].xyz, id);
			memoryBarrierShared();
			ray_pos[id].xyz += ray_dir[id]*d;
		}
	}
	
	for(int i = 0; i < bundle_length; i++)
	{
		pos[i] = ray_pos[id];
	}
}