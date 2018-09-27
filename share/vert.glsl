#version 330
#define M_PI 3.1415926535897932384626433832795

struct Dual { vec4 u, v; };

// Dual product
Dual mul(Dual u, Dual v);
// Dual 'sandwich' product
Dual sandwich(Dual u, Dual v);

//uniform mat4 projection;
uniform float a=1,
	r = 2, l = -2,
	t = 2, b = -2,
	n = 1, f = 10;

const vec4 u0 = vec4(1,0,0,0), v0 = vec4(0,0,0,0);
uniform Dual model = Dual(u0, v0), view = Dual(u0, v0);

in vec4 pos_in;
out vec4 pos_out;

vec4 project(vec4 A) {
	float asp = sqrt(a);
	// GL multiplication of vectors is parallel, which allows
	// a decomposition of the projection matrix into a MAD-friendly
	// sum of products, eliminating most of the zeroes.
	return 2*n*vec4(A.xyw, 1)*vec4(asp/r, 1/(asp*t), f/(n-f), 0)
			+ A.z*vec4(asp*(r+l)/(r-l), (t+b)/(asp*(t-b)), (n+f)/(n-f), -1);
}

void main(){
	Dual x0 = Dual(u0, vec4(0, pos_in.xyz)),
		s = sandwich(mul(view, model), x0),
	float aspect = sqrt(a);
	gl_Position = project(vec4(s.v.yzw, pos_in.w));
	pos_out = project(vec4(x0.v.yzw, pos_in.w));
}
