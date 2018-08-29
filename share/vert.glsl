#version 330

uniform mat4 projection;

struct Dual;
vec4 qmul(vec4 l, vec4 r);

uniform Dual model, view;

uniform vec4 u = vec4(1, 1, 0, 0), v = vec4(1, 0, 1, 0);

in vec4 pos;
out vec4 w;

void main(){
    gl_Position = projection * pos;
	w = vec4(1, 1, 1, 1);
	//w = qmul(u, v);
	// w = qsandwich(u, v);
}
