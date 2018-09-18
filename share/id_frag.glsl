#version 330
#define M_PI 3.1415926535897932384626433832795

in vec4 pos_out;

out vec4 color;

float renorm(float x) {
	return x*0.5 + 0.5;
}
float ncos(float x) {
	return renorm(cos(x));
}
float nsin(float x) {
	return renorm(sin(x));
}

void main(){
	int id = gl_PrimitiveID & 255;
	//int bits[8] = int[8](id&1, (id>>1)&1, (id>>2)&1, (id>>3)&1,
			//(id>>4)&1, (id>>5)&1, (id>>6)&1, (id>>7)&1);
	float theta = (id&15)*M_PI/4;
	color = vec4(nsin(theta), nsin(theta+M_PI*2/3), nsin(theta+M_PI*4/3), 1);
}
