#version 430
//4*4 ray bundle
#define bundle_size 1
#define bundle_length 1
#define bundle_center 0
#define group_size 4
#define block_size 16
#define MAX_MARCHES 128
#define RBM 0
#include<camera.glsl>

//4*4 bundle of ray bundles(oh lol)
layout(local_size_x = group_size, local_size_y = group_size) in;
layout(rgba8, binding = 0) uniform image2D img_output;

//make all the local distance estimator spheres shared
shared vec4 de_sph[block_size]; 
float DE_count = 0;
#include<ray_marching.glsl>


//each shader invocation will march a ray bundle,
//all invocations will have all the DE data from neighbore
//ray bundles to improve marching efficiency
//since it will give a larger volume of space in which the rays can safely march
//(not only a single DE sphere)

void main() {
	ivec2 global_pos = ivec2(gl_GlobalInvocationID.x,gl_GlobalInvocationID.y);
	int local_indx = int(gl_LocalInvocationIndex);
	ivec2 block_pos = bundle_size*global_pos;
	vec2 img_size = vec2(imageSize(img_output));
	if(block_pos.x < img_size.x && block_pos.y < img_size.y)
	{
		//ray bundle array
		vec4 pos[bundle_length];
		vec4 dir[bundle_length];
		vec4 var[bundle_length];
		
		//initialize the ray bundle
		ivec2 pix = block_pos;
		#pragma unroll
		for(int i = 0; i < bundle_length; pix = block_pos + getLPos(++i))
		{	
			ray rr = get_ray(vec2(pix)/img_size);
			pos[i] = vec4(rr.pos,0);
			dir[i] = vec4(rr.dir,0);
			var[i] = vec4(0);
		}
		
		//de_sph[local_indx] = pos[bundle_center]; 
		
		memoryBarrierShared();
		
		#if RBM
			ray_bundle_marching1(pos, dir, var, local_indx);
		#else
			#pragma unroll
			for(int i = 0; i < bundle_length; i++)
			{
				ray_march(pos[i], dir[i], var[i], fovray);
			}
		#endif
		
		// output to the specified image block
		
		pix = block_pos;
		#pragma unroll
		for(int i = 0; i < bundle_length; pix = block_pos + getLPos(++i))
		{	
			//ray rr = get_ray(vec2(pix)/vec2(imageSize(img_output)));
			
		/*	pos[i] = vec4(rr.pos,0);
			dir[i] = vec4(rr.dir,0);
			ray_march(pos[i], dir[i]);*/
			float DE_per_pix = DE_count/float(bundle_length*MAX_MARCHES);
			float ao = (1 -  var[i].x/MAX_MARCHES);
			float td = (1 - dir[i].w*0.06f);
			//memoryBarrierImage();
			imageStore(img_output, pix, vec4(DE_per_pix,DE_per_pix,DE_per_pix,1));	  
		}
	}
}