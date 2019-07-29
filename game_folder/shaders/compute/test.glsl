#version 430
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba8, binding = 0) uniform image2D img_output;

//placeholder shader
void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);

	float max_x = 5.0;
	float max_y = 5.0;
	ivec2 dims = imageSize(img_output); // fetch image dimensions
	float x = (float(pixel_coords.x * 2 - dims.x) / dims.x);
	float y = (float(pixel_coords.y * 2 - dims.y) / dims.y);
	vec3 ray_o = vec3(x * max_x, y * max_y, 0.0);
	vec3 ray_d = vec3(0.0, 0.0, -1.0); // ortho

	vec3 sphere_c = vec3(0.0, 0.0, -10.0);
	float sphere_r = 1.0;
	
	vec3 omc = ray_o - sphere_c;
	float b = dot(ray_d, omc);
	float c = dot(omc, omc) - sphere_r * sphere_r;
	float bsqmc = b * b - c;
	// hit one or both sides
	if (bsqmc >= 0.0) {
		pixel = vec4(0.4, 0.4, 1.0, 1.0);
	}
	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}