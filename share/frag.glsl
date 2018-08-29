#version 330

in vec4 arg0;
in vec4 w;

out vec4 color;

void main(){
	//color = vec4(.5+.5*sin(gl_FragCoord[0]/100), .5+.5*sin(gl_FragCoord[1]/100), .5, 1);
	vec3 normed = normalize(vec3(w.xyz))*0.5 + vec3(0.5, 0.5, 0.5);
}
