uniform sampler2D buffer;
uniform sampler2D de_distor;
void main(void){
	vec4 dc = gl_TexCoord[0]; dc[3] = 1.0;
	vec4 dd = texture2DProj(de_distor, gl_TexCoord[0]); dd[3] = 0.0;
	if(0.0 < dd[0]){
		dd[0] = (dd[0] * 256.0 - 128.5) / 1280.0;
		dd[1] = (dd[1] * 256.0 - 128.5) / 800.0;
		gl_FragColor = texture2DProj(buffer,dc + dd);
	}else{
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
