#version 430
//4*4 ray bundle
#define group_size 8
#define block_size 64
#define RBM1 0

layout(local_size_x = group_size, local_size_y = group_size) in;
layout(rgba32f, binding = 0) uniform image2D DE_input; 
layout(rgba32f, binding = 1) uniform image2D DE2_input;
layout(rgba32f, binding = 2) uniform image2D var_input; 
layout(rgba32f, binding = 3) uniform image2D DE_output; //calculate final DE spheres
layout(rgba32f, binding = 4) uniform image2D DE2_output;
layout(rgba32f, binding = 5) uniform image2D var_output; 

//make all the local distance estimator spheres shared
shared vec4 de_sph[group_size][group_size]; 

#include<camera.glsl>
#include<shading.glsl>

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
	vec4 sph_norm = imageLoad(DE2_input, prev_pos);
	
	#if(RBM1)
		de_sph[local_indx.x][local_indx.y] = sph;
		memoryBarrierShared(); 
	#endif
	
	ray rr = get_ray(vec2(global_pos)/img_size);
	vec4 pos = vec4(rr.pos,0);
	vec4 dir = vec4(rr.dir,0);
	vec4 var = imageLoad(var_input, prev_pos);
		
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
		d = max(d, sphere_intersection(dir.xyz, pos.xyz, sph_norm));
	#endif
	
	pos.w = d;
	var.w = 1;
	
	fovray = 1*Camera.FOV/img_size.x;
	
	ray_march(pos, dir, var, fovray, 0);
	
	//save the DE spheres
	imageStore(DE_output, global_pos, pos);	 
	imageStore(DE2_output, global_pos, vec4(0));	 
	imageStore(var_output, global_pos, var);				
}