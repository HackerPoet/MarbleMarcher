#include<distance_estimators.glsl>

#define MAX_DIST 50
#define MIN_DIST 0.01
float FOVperPixel = 0.001;

void ray_march(inout ray r) 
{
	//March the ray
	float d = DE(vec4(get_p(r),1));
	for (float s = 0; s < MAX_MARCHES; s += 1.0) {
		//if the distance from the surface is less than the distance per pixel we stop
		float min_dist = max(r.fov*r.td, MIN_DIST);
		if(r.td > MAX_DIST || d < min_dist)
		{
			break;
		}
		r.td += d;
		d = DE(vec4(get_p(r),1));
	}
}


float sphere_intersection(vec3 r, vec3 p, vec3 sp, float R)
{
	vec3 dp = p - sp;
	float b = 2*dot(dp, r);
	float c = dot(dp,dp) - R*R;
	float D = b*b - 4*c;
	if(D < 0 || c > 0) //if no intersection or outside the sphere
	{
		return 0;
	}
	else
	{
		return (sqrt(D) - b)*0.5f; //use furthest solution in the direction of the ray
	}
}

float find_furthest_d(vec3 r, vec3 p, ivec2 pi)
{
	float d = 0;
	/*//check through all of the local bundles
	for(int i = 0; i < group_size; i++)
	{
		for(int j = 0; j < group_size; j++)
		{
			if(DE_data[i][j] > 0) // if the ray has a DE
			{
				int ix = bundle_size*i + bundle_size/2;
				int iy = bundle_size*j+ bundle_size/2;
				float this_d = sphere_intersection(r, p, ray_array[ix][iy].pos, DE_data[ix][iy]);
				d = max(d, this_d);
			}
		}
	}*/
	//check this one and the central one
	d = max(d,sphere_intersection(r, p, get_p(ray_array[pi.x][pi.y]), DE_data[pi.x][pi.y]));
	
	int cx = pi.x/bundle_size + bundle_size/2;
	int cy = pi.y/bundle_size + bundle_size/2;
	d = max(d, sphere_intersection(r, p, get_p(ray_array[cx][cy]), DE_data[cx][cy]));
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
}