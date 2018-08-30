#version 330
#define M_PI 3.1415926535897932384626433832795

uniform mat4 projection;

vec4 qconj(vec4);
vec4 qmul(vec4, vec4);
vec4 qsandwich(vec4, vec4);

struct Dual { vec4 u; vec4 v; };
Dual dmul(Dual, Dual);
Dual dconj(Dual);
Dual dsandwich(Dual, Dual);

Dual dqmul(Dual, vec4);
Dual qdmul(vec4, Dual);
Dual qdsandwich(vec4, Dual);

uniform Dual model = Dual(vec4(cos(M_PI/8),0,0,sin(M_PI/8)), vec4(0,0,0.25,0)), view;
uniform vec4 u = vec4(cos(M_PI/8),0,sin(M_PI/8),0), v;

in vec4 pos_in;
out vec4 pos_out;

void main(){
	Dual x = Dual(vec4(1,0,0,0), vec4(0, pos_in.xyz));
	Dual y = dsandwich(Dual(u, vec4(0,0,0.25,0)), x);
	//Dual y = dsandwich(model, x);
	gl_Position = projection * vec4(y.v.yzw, 1);
    pos_out = gl_Position;
}
