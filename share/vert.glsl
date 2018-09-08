#version 330
#define M_PI 3.1415926535897932384626433832795

struct Dual { vec4 u, v; };

// Dual 'sandwich' product
Dual sandwich(Dual u, Dual v);

//uniform mat4 projection;
uniform float a=1,
	r = 2, l = -2,
	t = 2, b = -2,
	n = 1, f = 10;

const vec4 u0 = vec4(1,0,0,0), v0 = vec4(0,0,0,0);
uniform Dual model = Dual(u0, v0);

in vec4 pos_in;
out vec4 pos_out;

void main(){
	Dual x0 = Dual(u0, vec4(0, pos_in.xyz)),
		s = sandwich(model, x0);
	float aspect = sqrt(a);
	//gl_Position = 2*n*vec4(1/r, 1/t, f/(n-f), 0) * vec4(s.v.yz, pos_in.w, 1)
	//	+ s.v.w * vec4((r+l)/(r-l), (t+b)/(t-b), (n+f)/(n-f), -1);
	gl_Position = 2*n*vec4(aspect/r, 1/(aspect*t), f/(n-f), 0) * vec4(s.v.yz, pos_in.w, 1)
		+ s.v.w * vec4(aspect*(r+l)/(r-l), (t+b)/(aspect*(t-b)), (n+f)/(n-f), -1);
	pos_out = gl_Position;
}
