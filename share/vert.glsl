#version 330
#define M_PI 3.1415926535897932384626433832795

struct Dual { vec4 u, v; };
Dual sandwich(Dual u, Dual v);

const vec4 u0 = vec4(1,0,0,0), v0 = vec4(0,0,0,0);
//uniform vec4 model[2] = {u0, v0};
uniform vec4 model[2] = {vec4(0, 1, 0, 0), vec4(0, 0, 0, -4)};

// projection[2] = {vec4(1, 1, -2.2, 0), vec4(0, 0, -1.2, -1)};

in vec4 pos_in;
out vec4 pos_out;

void main(){
//	float right = 10, left = -right,
//		top = 10, bottom = -top,
//		near = 1, far = 10;
//	mat4 projection = mat4(
//		2*near/right, 0, (right + left) / (right - left), 0,
//		0, 2*near/top, (top + bottom) / (top - bottom), 0,
//		0, 0, 2*near*far/(near-far), (near+far)/(near-far),
//		0, 0, -1, 0);

	mat4 projection = mat4(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0,-1.2,-2.2,
		0.0, 0.0,-1.0, 0.0
	);
	Dual d = sandwich(Dual(model[0], model[1]), Dual(u0, vec4(0, pos_in.xyz)));
	vec4 newpos = vec4(d.v.yzw, pos_in.w);
	//vec4 newpos = vec4(d.v.yzw, 1);
	gl_Position = projection * newpos;
	//gl_Position = proj0 * newpos + proj1 * newpos
		//+ proj2 * newpos + proj3 * newpos;



	//gl_Position = proj * pos_in;
	//  x = d.v.w; y = d.v.x; z = d.v.y; w = pos_in.w
	//Dual d = sandwich(Dual(model[0], model[1]), Dual(u0, vec4(0, pos_in.xyz)));
	//gl_Position = projection[0] * vec4(d.v.wx, pos_in.w, 0) + d.v.y * projection[1];
	//gl_Position = projection[0] * vec4(pos_in.xyw, 0) + pos_in.z * projection[1];


//	float w = pos_in.w, x = d.v.y, y = d.v.z, z = d.v.w;
	// Aiming for multiply-add boost, and less operations than matrix
//	gl_Position = 2 * near * vec4( x / right, y / top, w * far / (near - far), 0)
//		+ z * vec4((right + left) / (right - left), (top + bottom) / (top - bottom),
//				(far + near) / (near - far), -1);

	pos_out = gl_Position;
}
