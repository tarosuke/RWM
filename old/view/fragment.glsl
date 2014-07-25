uniform sampler2D buffer;
uniform sampler2D de_distor;
void main(void){
	vec4 dd = texture2DProj(de_distor, gl_TexCoord[0]); dd[3] = 1.0;
	if(0.0 <= dd[0]){
		gl_FragColor = texture2DProj(buffer, dd);
	}else{
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
