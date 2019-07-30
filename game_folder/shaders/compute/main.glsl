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
			ray_array[gl_WorkGroupID.x + i][gl_WorkGroupID.y + j] = get_ray(screen_coords);
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
	
	bool bundle_complete = false;
	///A loop
	
	while(!bundle_complete)
	{
		//march the central rays
		
		//march the secondary rays
	}
	
	//do some rendering stuff

	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}