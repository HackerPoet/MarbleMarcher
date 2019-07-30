#version 430
//4*4 ray bundle
#define bundle_size 4

#include<camera.glsl>

//4*4 bundle of ray bundles(oh lol)
layout(local_size_x = 4, local_size_y = 4) in;
layout(rgba8, binding = 0) uniform image2D img_output;

//make all the local ray bundles shared
shared ray ray_array[local_size_x*bundle_size][local_size_y*bundle_size]; 
//make the distance estimate data shared
shared float DE_data[local_size_x*bundle_size][local_size_y*bundle_size]; 

#include<ray_marching.glsl>

//each shader invocation will march a ray bundle,
//all invocations will have all the DE data from neighbore
//ray bundles to improve marching efficiency
//since it will give a larger volume of space in which the rays can safely march
//(not only a single DE sphere)

void main() {
	// base pixel colour for image
	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	//initialize the ray bundle
	for(int i = 0; i < bundle_size; i++)
	{
		for(int j = 0; j < bundle_size; j++)
		{
			vec2 screen_coords = (bundle_size*gl_GlobalInvocationID.xy+vec2(i,j))/Camera.resolution;
			ray_array[gl_LocalInvocationID.x + i][gl_LocalInvocationID.y + j] = get_ray(screen_coords);
			DE_data[bundle_size*gl_LocalInvocationID.x + i][bundle_size*gl_LocalInvocationID.y + j] = 0;
		}	  
	}

	//wait untill all the workgroup data is written
	memoryBarrierShared();
	
	bool complete[bundle_size][bundle_size];
	for(int i = 0; i < bundle_size; i++)
	{
		for(int j = 0; j < bundle_size; j++)
		{
			complete[i][j] = false;
		}	  
	}
	
	bool bundle_computing = true;
	
	//central ray position
	int crx = bundle_size/2, cry = bundle_size/2;
	
	///Ray bundle marching
	while(bundle_computing)
	{
		bundle_computing = false;
		//march the central rays
		if(!complete[crx][cry])
		{
			complete[crx][cry] = march_ray_bundle(bundle_size*gl_LocalInvocationID.x + crx, bundle_size*gl_LocalInvocationID.y + cry);
			bundle_computing = bundle_computing || !complete[crx][cry];
		}
		
		//march the secondary rays
		for(int i = 0; i < bundle_size; i++)
		{
			for(int j = 0; j < bundle_size; j++)
			{
				if(i != crx && j != cry && !complete[i][j])
				{
					complete[i][j] = march_ray_bundle(bundle_size*gl_LocalInvocationID.x + i, bundle_size*gl_LocalInvocationID.y + j);
					bundle_computing = bundle_computing && !complete[i][j];
				}
			}	  
		}
	}
	
	//do some rendering stuff

	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}