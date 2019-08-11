#version 430
//4*4 ray bundle
#define group_size 8
#define block_size 64

layout(local_size_x = group_size, local_size_y = group_size) in;
layout(rgba32f, binding = 0) uniform image2D DE_input; 
layout(rgba32f, binding = 1) uniform image2D var_input; 
layout(rgba8, binding = 2) uniform image2D DE_output; //calculate final DE spheres


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
	de_sph[local_indx.x][local_indx.y] = imageLoad(DE_input, prev_pos);
	memoryBarrierShared();
	 
	ray rr = get_ray(vec2(global_pos)/img_size);
	vec4 pos = vec4(rr.pos,0);
	vec4 dir = vec4(rr.dir,0);
	vec4 var = interp(var_input, vec2(global_pos)/MRRM_step_scale);
		
	float td = length(pos.xyz - de_sph[local_indx.x][local_indx.y].xyz);//traveled distance
	
	//first order, MRRM
	pos.xyz += dir.xyz*td;//move local ray beginning inside the DE sphere;
	dir.w += td;
	
	//calculate new best pos, second order, MRRBM
	barrier();
	float d = find_furthest_intersection(dir.xyz, pos.xyz, local_indx);
	
	pos.xyz += dir.xyz*d; //move the ray beginning to the furthest sphere intersection
	dir.w += d;
	
	fovray = Camera.FOV/img_size.x;
	
	ray_march(pos, dir, var, fovray);

	float depth = (1 - dir.w*0.06f);
	float ao = (1 -  var.x/MAX_MARCHES);
	vec4 color;
	if(pos.w < 0.1)
	{
		vec4 norm = calcNormal(pos.xyz, pos.w/4);
		pos.xyz -= norm.xyz*norm.w;
		color = COL(pos.xyz);
	}
	else
	{
		color = vec4(1);
	}
	
	//save the DE sphere
	imageStore(DE_output, global_pos, vec4(color.xyz*ao,1));	  
}