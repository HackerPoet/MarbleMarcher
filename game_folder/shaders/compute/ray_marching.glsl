#include<distance_estimators.glsl>

#define MAX_DIST 50
#define MIN_DIST 0.001


void ray_march(inout vec4 pos, inout vec4 dir, inout vec4 var, float fov) 
{
	//March the ray
	float s = 0;
	for (; s < MAX_MARCHES; s += 1.0) {
		pos.w = DE(pos.xyz);
		//if the distance from the surface is less than the distance per pixel we stop
		if(dir.w > MAX_DIST || pos.w < max(fov*dir.w, MIN_DIST))
		{
			break;
		}
		dir.w += pos.w;
		pos.xyz += pos.w*dir.xyz;
	}
	var.x = s;
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
/*
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

float find_furthest_DE_local(vec3 r, vec3 p, int id, float tr)
{
	float d = 0;
	/*ivec2 id2 = getGpos(id);
	ivec2 id11 = max(id2 - 1,0);
	ivec2 id22 = min(id2 + 1,group_size);
	//check through all of the local bundles
	for(int i = id11.x; i < id22.x; i++)
	{
		for(int j = id11.y; j < id22.y; j++)
		{
			int index = i + group_size*j;
			if(ray_pos[index].w < 2*tr)
				d = max(sphere_intersection(r, p, ray_pos[index]), d);
		}
	}
	d = max(sphere_intersection(r, p, ray_pos[id]), d);
	return d;
}

float find_furthest_DE(vec3 r, vec3 p, int id)
{
	float d = 0;
	float tr = ray_pos[id].w;
	//check through all of the local bundles
	
	for(int i = 0; i < block_size; i++)
	{
		if(ray_pos[i].w < 2*tr)
			d = max(sphere_intersection(r, p, ray_pos[i]), d);
	}
	
	
//	d = max(sphere_intersection(r, p, ray_pos[id]), d);
	return d;
}

void ray_bundle_marching(inout vec4 pos[bundle_length], inout vec4 dir[bundle_length], int id)
{
	float bundle_fov = 4*bundle_size*fovray;
	float d = 0;
	bool marching = true;
	int l = 0;
	for(int m = 0; (m < MAX_MARCHES) && marching; m++)
	{
		memoryBarrierShared();
		ray_pos[id].w = DE(ray_pos[id].xyz);
		memoryBarrierShared();
		if(ray_pos[id].w < bundle_fov*ray_dir[id].w) 
		{
			marching = false;
			//march the ray bundle
			for(int i = 0; i < bundle_length; i++)
			{
				if(l == 0)
				{
					pos[i].xyz += dir[i].xyz*ray_dir[id].w;
					pos[i].w = d;
					dir[i].w = ray_dir[id].w;
				}
				
				if(pos[i].w > fovray*dir[i].w)
				{
					marching = true;
					d = find_furthest_DE_local(dir[i].xyz, pos[i].xyz, id, pos[i].w);
					//if found a usable DE
					if(true || d == 0)
					{
						pos[i].w = DE(pos[i].xyz);
						pos[i].xyz += dir[i].xyz*pos[i].w;
						dir[i].w += pos[i].w;
					}
					else
					{
						pos[i].xyz += dir[i].xyz*d;
						dir[i].w += d;
					}
				}
			}	
			l++;
		}
		else if(ray_dir[id].w > MAX_DIST)
		{
			for(int i = 0; i < bundle_length; i++)
			{
				pos[i] = ray_pos[id];
				dir[i] = ray_dir[id];
			}
			break;
		}
		else
		{
			d = find_furthest_DE_local(ray_dir[id].xyz, ray_pos[id].xyz, id, ray_pos[id].w);
			memoryBarrierShared();
			ray_dir[id].w += d;
			ray_pos[id].xyz += ray_dir[id].xyz*d;
		}
	}
}
*/

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
}