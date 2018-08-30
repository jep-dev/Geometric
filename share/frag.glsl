#version 330
#define M_PI 3.1415926535897932384626433832795

in vec4 pos_out;

out vec4 color;
void main(){
	color = pos_out * pos_out / length(pos_out.xyz);

	//vec3 normed = vec3(w.y*w.y, w.z*w.z, w.w*w.w);
	//vec3 normed = normalize(vec3(w.yzw))*0.5 + vec3(0.5, 0.5, 0.5);
	//color = vec4(normed, 1);
	//color = vec4(w.yzw, 1);
}
