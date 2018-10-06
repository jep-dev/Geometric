#version 330
#define M_PI 3.1415926535897932384626433832795

in vec4 pos;
in vec2 uv;

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
	float x = pos.x*2*M_PI, y = pos.y*2*M_PI, z = pos.z*2*M_PI,
		cx = ncos(x*2*M_PI), cy = ncos(y*2*M_PI), cz = ncos(z*2*M_PI),
		x2 = x*x, y2 = y*y, z2 = z*z,
		xy2 = x2+y2, xz2 = x2+z2, yz2 = y2+z2, xyz2 = xy2+z2,
		xy = sqrt(xy2), xz = sqrt(xz2), yz = sqrt(yz2),
			cxy = ncos(xy*M_PI*2), cyz = ncos(yz*M_PI*2), cxz = ncos(xz*M_PI*2),
			sxy = nsin(xy*M_PI*2), syz = nsin(yz*M_PI*2), sxz = nsin(xz*M_PI*2),
		xyz = sqrt(xyz2), cxyz = ncos(xyz*M_PI*2),
		h = M_PI*(cos(x)+cos(y)+cos(z)),
		b = cos(2*atan2(cx,(cx*cx+cy*cy+cz*cz))*(cos(uv.x*2*M_PI)+sin(uv.y*2*M_PI)));
	//color = vec4(uv.x, uv.y, sqrt(uv.x*uv.x+uv.y*uv.y), 1);
	color = vec4(ncos(h)*b, ncos(h+M_PI*2/3)*b, ncos(h+M_PI*4/3)*b, 1);
	//color = vec4(ncos(h), ncos(h+M_PI*2/3), ncos(h+M_PI*4/3), 1);
}
