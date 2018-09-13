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
	float x = pos_out.x, y = pos_out.y, z = pos_out.z,
			ax = abs(x), ay = abs(y), az = abs(z),
			axyz = ax*ax+ay*ay+az*az,
			ncx = ncos(x*M_PI*2), ncy = ncos(y*M_PI*2), ncz = ncos(z*M_PI*2),
			pcx = 1-pow(ncx,32), pcy = 1-pow(ncy,32), pcz = 1-pow(ncz,32),
			pxyz = pcx*pcy*pcz;
			color = vec4(x-2, ay/axyz, az/axyz, 1);
	float c = ncos(gl_PrimitiveID), s = nsin(gl_PrimitiveID);
	color = vec4(c, s, (c+s)/2, 1);
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
