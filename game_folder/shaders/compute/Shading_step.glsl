#version 430
//4*4 ray bundle
#define group_size 8
#define block_size 64

layout(local_size_x = group_size, local_size_y = group_size) in;
layout(rgba32f, binding = 0) uniform image2D DE_input; 
layout(rgba32f, binding = 1) uniform image2D DE2_input;
layout(rgba32f, binding = 2) uniform image2D var_input; 
layout(rgba8, binding = 3) uniform image2D color_output; //calculate final color


//make all the local distance estimator spheres shared
shared vec4 de_sph[group_size][group_size]; 

#include<camera.glsl>
#include<shading.glsl>

///The second step of multi resolution ray marching

void main() {
	ivec2 global_pos = ivec2(gl_GlobalInvocationID.xy);
	ivec2 local_indx = ivec2(gl_LocalInvocationID.xy);
	vec2 img_size = vec2(imageSize(color_output));
	
	vec4 sph = imageLoad(DE_input, global_pos);
	vec4 sph_norm = imageLoad(DE2_input, global_pos);
	
	ray rr = get_ray(vec2(global_pos)/img_size);
	vec4 pos = vec4(rr.pos,0);
	vec4 dir = vec4(rr.dir,0);
	vec4 var = imageLoad(var_input, global_pos);
	
	float td = dot(dir.xyz, sph.xyz - pos.xyz);//traveled distance
	
	pos = sph;
	dir.w += td; 
	
	vec4 color;
	if(pos.w<1)
	{
		color = shading(pos, dir, fovray);
	}
	else
	{
		color = BACKGROUND_COLOR;
	}
	
	imageStore(color_output, global_pos, HDRmapping(color.xyz, Camera.exposure, 2.2));	  
}