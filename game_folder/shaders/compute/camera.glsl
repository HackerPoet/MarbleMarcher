
struct gl_camera
{
	vec3 position;
	vec3 dirx;
	vec3 diry;
	vec3 dirz;
	vec2 resolution;
	float aspect_ratio;
	float FOV;
	float focus;
	float bokeh;
	float exposure;
	float mblur;
	float speckle;
	float size;
	int stepN;
	int step;
};

struct ray
{
	vec3 pos;
	vec3 dir;
	float td; //traveled distance
	float fov; //field of view(cone angle)
	
	vec3 get_p()
	{
		return pos + dir*td;
	}
};

uniform gl_camera Camera;

ray get_ray(vec2 screen_pos)
{
	vec2 shift = Camera.FOV*(2*screen_pos - 1)*vec2(aspect_ratio, 1/aspect_ratio);
	ray cray;
	cray.pos = Camera.position;
	cray.dir = normalize(diry + dirx*shift.x + dirz*shift.y);
	cray.td = 0;
	cray.fov = Camera.FOV/Camera.resolution.x; //pixel FOV
	return cray;
}