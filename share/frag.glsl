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
float atan2(float y, float x) {
	if(x == 0)
		if(y > 0) return M_PI/2;
		else return -M_PI/2;
	else return atan(y/x);
}

void main(){
	float x = pos_out.x, y = pos_out.y, z = pos_out.z,
			ax = abs(x), ay = abs(y), az = abs(z),
			rxz = sqrt(ax*ax+az*az),
			t = atan2(rxz, ay)*16,
			//t = (atan(rxz, ay)*4)*8,
			t2 = t + M_PI*2/3, t3 = t + M_PI*4/3;
	color = vec4(nsin(t), nsin(t2), nsin(t3), exp(-z*z));
	//color = vec4(nsin(s*64), nsin(s*64+M_PI*2/3), nsin(s*64+M_PI*4/3), 1);


	//color = vec4(ax/axyz, ay/axyz, az/axyz, 1);
	//color = vec4(ax*pcx, ay*pcy, az*pcz, 1);
	//color = vec4(abs(pos_out.x)/4, abs(pos_out.y)/4, abs(pos_out.z)/4, 1);
	//color = vec4(renorm(cos(pos_out.x*M_PI)),
		//renorm(cos(pos_out.y*M_PI)),
		//renorm(cos(pos_out.z*M_PI)), 1);
	//color = pos_out * pos_out / length(pos_out.xyz);

	//vec3 normed = vec3(w.y*w.y, w.z*w.z, w.w*w.w);
	//vec3 normed = normalize(vec3(w.yzw))*0.5 + vec3(0.5, 0.5, 0.5);
	//color = vec4(normed, 1);
	//color = vec4(w.yzw, 1);
}
