
struct ray
{
	vec3 pos;
	vec3 dir;
};		

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


ivec2 getGpos(int index)
{
	int y = index/group_size;
	int x = index%group_size;
	return ivec2(x,y);
}

uniform gl_camera Camera;
float fovray;

ray get_ray(vec2 screen_pos)
{
	vec2 shift = Camera.FOV*(2.f*screen_pos - 1.f)*vec2(Camera.aspect_ratio, 1.f);
	ray cray;
	cray.pos = Camera.position;
	cray.dir = normalize(Camera.dirx*shift.x + Camera.diry*shift.y + Camera.dirz);
	fovray = Camera.FOV/Camera.resolution.x; //pixel FOV
	return cray;
}