#version 430
//4*4 ray bundle
#define bundle_size 4
#define bundle_length 16
#define bundle_center 8
#define group_size 8
#define block_size 64
#define MAX_MARCHES 128
#include<camera.glsl>

//4*4 bundle of ray bundles(oh lol)
layout(local_size_x = group_size, local_size_y = group_size) in;
layout(rgba8, binding = 0) uniform image2D img_output;

//make all the local distance estimator spheres shared
shared vec4 de_sph[block_size]; 

#include<ray_marching.glsl>

//each shader invocation will march a ray bundle,
//all invocations will have all the DE data from neighbore
//ray bundles to improve marching efficiency
//since it will give a larger volume of space in which the rays can safely march
//(not only a single DE sphere)

void main() {
	ivec2 global_pos = ivec2(gl_GlobalInvocationID.x,gl_GlobalInvocationID.y);
	int local_indx = int(gl_LocalInvocationIndex);
	
	//ray bundle array
	vec4 pos[bundle_length];
	vec4 dir[bundle_length];
	vec4 var[bundle_length];
	
	//initialize the ray bundle
	for(int i = 0; i < bundle_length; i++)
	{
		int index = bundle_size*local_indx + i;
		ivec2 pix = bundle_size*global_pos + getLPos(i);
		ray rr = get_ray(vec2(pix)/vec2(imageSize(img_output)));
		pos[i] = vec4(rr.pos,0);
		dir[i] = vec4(rr.dir,0);
		var[i] = vec4(0);
	}
	
	de_sph[local_indx] = pos[bundle_center]; 
	
	memoryBarrierShared();
	
	ray_bundle_marching1(pos, dir, var, local_indx);
	
	/*for(int i = 0; i < bundle_length; i++)
	{
		ray_march(pos[i], dir[i], var[i]);
	}*/
	
	// output to the specified image block
	for(int i = 0; i < bundle_length; i++)
	{
		ivec2 pix = bundle_size*global_pos + getLPos(i);
		ray rr = get_ray(vec2(pix)/vec2(imageSize(img_output)));
		
	/*	pos[i] = vec4(rr.pos,0);
		dir[i] = vec4(rr.dir,0);
		ray_march(pos[i], dir[i]);*/
		
		float ao = (1 -  var[i].x/MAX_MARCHES);
		float td = (1 - dir[i].w*0.06f);
		vec3 depth = vec3(td,td,td);
		if(isinf(td))
		{
			depth = vec3(td,td,ao);
		}
		
		imageStore(img_output, pix, vec4(depth, 1));	  
	}
}