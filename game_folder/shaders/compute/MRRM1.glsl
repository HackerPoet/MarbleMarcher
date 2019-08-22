#version 430
//4*4 ray bundle
#define group_size 8
#define block_size 64

layout(local_size_x = group_size, local_size_y = group_size) in;
layout(rgba32f, binding = 0) uniform image2D DE_output; //calculate final DE spheres
layout(rgba32f, binding = 1) uniform image2D DE2_output;
layout(rgba32f, binding = 2) uniform image2D var_output; 

//make all the local distance estimator spheres shared
shared vec4 de_sph[group_size][group_size]; 

#include<camera.glsl>
#include<ray_marching.glsl>

///The first step of multi resolution ray marching

void main() {
	ivec2 global_pos = ivec2(gl_GlobalInvocationID.x,gl_GlobalInvocationID.y);
	int local_indx = int(gl_LocalInvocationIndex);
	vec2 img_size = vec2(imageSize(DE_output));
	//if within the texture
	
	//initialize the ray
	ray rr = get_ray(vec2(global_pos)/img_size);
	vec4 pos = vec4(rr.pos,0);
	vec4 dir = vec4(rr.dir,0);
	vec4 var = vec4(0);
	
	fovray = 1.2*Camera.FOV/img_size.x;
	
	ray_march(pos, dir, var, fovray, 10*fovray);
	
	vec4 pos1 = pos;
	
	//normarch(pos1);
	
	//save the DE spheres
	imageStore(DE_output, global_pos, pos);	 
	imageStore(DE2_output, global_pos, pos1);	 
	imageStore(var_output, global_pos, var);			
}