#version 330

uniform mat4 projection;
uniform vec4 u = vec4(1, 0, 0, 0);
uniform vec4 v = vec4(1, 0, 0, 0);

//struct Dual;
vec4 qmul(vec4 l, vec4 r);
vec4 qsandwich(vec4 l, vec4 r);
//uniform Dual model, view;


in vec4 pos_in;
out vec4 pos_out;
out vec4 w;

void main(){
	gl_Position = projection * pos_in;
    pos_out = gl_Position;

	// w = u;
	// w = qmul(u, v);
	w = qsandwich(u, v);
}
