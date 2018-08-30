#version 330
#define M_PI 3.1415926535897932384626433832795

uniform mat4 projection;

vec4 conj(vec4);
vec4 mul(vec4, vec4);
vec4 sandwich(vec4, vec4);

struct Dual { vec4 u; vec4 v; };
Dual mul(Dual, Dual);
Dual conj(Dual);
Dual sandwich(Dual, Dual);

Dual mul(Dual, vec4);
Dual mul(vec4, Dual);
Dual sandwich(vec4, Dual);

const vec4 model_u = vec4(cos(M_PI/8),0,sin(M_PI/8),0), model_v = vec4(0, 0, 0.25, 0);
uniform vec4 u = model_u, v = model_v;
uniform Dual model = Dual(model_u, model_v), view;

in vec4 pos_in;
out vec4 pos_out;

void main(){
	Dual x = Dual(vec4(1,0,0,0), vec4(0, pos_in.xyz)),
		mxm = sandwich(Dual(u, v), x);
	gl_Position = projection * vec4(mxm.v.yzw, 1);
    pos_out = gl_Position;
}
