#version 120

uniform vec2 iResolution;
uniform sampler2D render_texture; 

void main() {
	vec2 screen_pos = gl_FragCoord.xy / iResolution.xy;
	gl_FragColor = texture2D(render_texture, screen_pos);
}
