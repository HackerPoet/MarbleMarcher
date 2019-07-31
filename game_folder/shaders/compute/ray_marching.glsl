#include<distance_estimators.glsl>

//1 marching iteration, true if converged
bool march_ray(inout ray r, inout float de)
{
	//if the distance from the surface is less than the distance per pixel we stop
	if (de < r.fov*r.td)
	{
		return true;
	} 
	r.td += de;
	de = DE(vec4(r.get_p(),1));
	return false;
}

void ray_marching(inout ray r)
{
	float de = DE(vec4(r.get_p(),1));
	for(int i = 0; i < MAX_MARCHES; i++)
	{
		if(march_ray(r, de))
		{
			break;
		}
	}
}

#define MAX_DIST 1000
#define MIN_DIST 0.001
float FOVperPixel = 0.001;

vec4 ray_march(inout vec4 p, vec4 ray, float sharpness) 
{
	//March the ray
	float d = DE(p);
	float s = 0.0;
	float td = 0.0;
	float min_d = 1.0;
	for (; s < MAX_MARCHES; s += 1.0) {
		//if the distance from the surface is less than the distance per pixel we stop
		float min_dist = max(FOVperPixel*td, MIN_DIST);
		if(d < -min_dist || td > MAX_DIST)
		{
			break;
		}
		else if (d < min_dist)
		{
			s += d / min_dist;
			break;
		} 
		td += d+min_dist*0.1;
		p += ray * (d+min_dist*0.1);
		min_d = min(min_d, sharpness * d / td);
		d = DE(p);
	}
	return vec4(d, s, td, min_d);
}


float sphere_intersection(vec3 r, vec3 p, vec3 sp, float R)
{
	vec3 dp = p - sp;
	float a = dot(r,r);
	float b = 2*dot(dp, r);
	float c = dot(dp,dp) - R*R;
	float D = b*b - 4*a*c;
	if(D < 0 || c > 0) //if no intersection or outside the sphere
	{
		return 0;
	}
	else
	{
		return (sqrt(D) - b)/(2*a); //use furthest solution in the direction of the ray
	}
}

float find_furthest_d(vec3 r, vec3 p)
{
	float d = 0;
	//check through all of the local bundles, may be slow(256 rays), so I'll probably change it
	for(int i = 0; i < block_size; i++)
	{
		for(int j = 0; j < block_size; j++)
		{
			if(DE_data[i][j] > 0) // if the ray has a DE
			{
				float this_d = sphere_intersection(r, p, ray_array[i][j].pos, DE_data[i][j]);
				d = max(d, this_d);
			}
		}
	}
	return d;
}

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
	
	float new_march = find_furthest_d(ray_array[i][j].dir, ray_array[i][j].get_p());
	
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
		DE_data[i][j] = DE(vec4(ray_array[i][j].get_p(),1));
	}
	return false;
}