#version 430
//4*4 ray bundle
#define bundle_size 4
#define bundle_length 16
#define bundle_center 8
#define group_size 4
#define block_size 16
#define MAX_MARCHES 128
#include<camera.glsl>

//4*4 bundle of ray bundles(oh lol)
layout(local_size_x = group_size, local_size_y = group_size) in;
layout(rgba8, binding = 0) uniform image2D img_output;

//make all the local ray bundle centers shared
shared vec3 ray_dir[block_size]; 
//positions and DE(.w)
shared vec4 ray_pos[block_size]; 

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
	vec3 dir[bundle_length];
	
	//initialize the ray bundle
	for(int i = 0; i < bundle_length; i++)
	{
		int index = bundle_size*local_indx + i;
		ivec2 pix = bundle_size*global_pos + getLPos(i);
		ray rr = get_ray(vec2(pix)/vec2(imageSize(img_output)));
		pos[i] = vec4(rr.pos,0);
		dir[i] = rr.dir;
	}
	
	//central ray
	ray_pos[local_indx] = pos[bundle_center]; 
	ray_dir[local_indx] = dir[bundle_center]; 
	
	memoryBarrierShared();
	
	ray_bundle_marching(pos, dir, local_indx);
	
	/*for(int i = 0; i < bundle_length; i++)
	{
		ray_march(pos[i], dir[i]);
	}*/
	
	// output to the specified image block
	for(int i = 0; i < bundle_length; i++)
	{
		ivec2 pix = bundle_size*global_pos + getLPos(i);
		ray rr = get_ray(vec2(pix)/vec2(imageSize(img_output)));
		
		float td = length(rr.pos - pos[i].xyz);
		vec3 depth = 1.f - vec3(td,td,td)/10.f;
		
		imageStore(img_output, pix, vec4(depth, 1));	  
	}
}