#version 430
//4*4 ray bundle
#define group_size 8
#define block_size 64
#define RBM1 0

layout(local_size_x = group_size, local_size_y = group_size) in;
layout(rgba32f, binding = 0) uniform image2D DE_input; 
layout(rgba32f, binding = 1) uniform image2D var_input; 
layout(rgba32f, binding = 2) uniform image2D DE_output; 
layout(rgba32f, binding = 3) uniform image2D var_output; 

//make all the local distance estimator spheres shared
shared vec4 de_sph[group_size][group_size]; 

#include<camera.glsl>
#include<ray_marching.glsl>

///The second step of multi resolution ray marching

void main() {
	ivec2 global_pos = ivec2(gl_GlobalInvocationID.xy);
	ivec2 local_indx = ivec2(gl_LocalInvocationID.xy);
	vec2 img_size = vec2(imageSize(DE_output));
	vec2 pimg_size = vec2(imageSize(DE_input));
	vec2 MRRM_step_scale = img_size/pimg_size;
	//if within the texture
	
	ivec2 prev_pos = min(ivec2((vec2(global_pos)/MRRM_step_scale) + 0.5),ivec2(pimg_size)-1);
	//initialize the ray
	vec4 sph = imageLoad(DE_input, prev_pos);
	
	#if(RBM1)
		de_sph[local_indx.x][local_indx.y] = sph;
		memoryBarrierShared(); 
	#endif
	
	ray rr = get_ray(vec2(global_pos)/img_size);
	vec4 pos = vec4(rr.pos,0);
	vec4 dir = vec4(rr.dir,0);
	vec4 var = interp(var_input, vec2(global_pos)/MRRM_step_scale);
		float vv = var.x;
	float td = dot(dir.xyz, sph.xyz - pos.xyz);//traveled distance
	
	//first order, MRRM
	pos.xyz += dir.xyz*td;//move local ray beginning inside the DE sphere	
	dir.w += td; 
	
	//calculate new best pos, second order, MRRBM
	#if(RBM1)
		barrier();
		float d = find_furthest_intersection(dir.xyz, pos.xyz, local_indx);
	#else
		float d = sphere_intersection(dir.xyz, pos.xyz, sph);
	#endif
	
	pos.w = d;
	var.w = 1;
	
	fovray = Camera.FOV/img_size.x;
	
	ray_march(pos, dir, var, fovray, fovray);

	//var = interp(var_input, vec2(global_pos)/MRRM_step_scale);
	//save the DE sphere
	imageStore(DE_output, global_pos, pos);	  
	imageStore(var_output, global_pos, var);		
}