#version 330

in vec4 arg0;
out vec4 arg1;

void main(){
    /* arg1 = vec4(0.6, 0.4, 0.1, 1.0); */
	arg1 = vec4(.5+.5*sin(gl_FragCoord[0]/100), .5+.5*sin(gl_FragCoord[1]/100), .5, 1);
}
