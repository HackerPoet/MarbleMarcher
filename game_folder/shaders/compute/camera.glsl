
struct ray
{
	vec3 pos;
	vec3 dir;
	float td; //traveled distance
	float fov; 
};

vec3 get_p(ray r)
{
	return r.pos + r.dir*r.td;
}			

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


uniform gl_camera Camera;

ray get_ray(vec2 screen_pos)
{
	vec2 shift = Camera.FOV*(2.f*screen_pos - 1.f)*vec2(Camera.aspect_ratio, 1.f);
	ray cray;
	cray.pos = Camera.position;
	cray.dir = normalize(Camera.dirz + Camera.dirx*shift.x + Camera.diry*shift.y);
	cray.td = 0;
	cray.fov = Camera.FOV/Camera.resolution.x; //pixel FOV
	return cray;
}