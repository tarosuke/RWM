uniform sampler2D buffer;
uniform sampler2D de_distor;

void main(void){
	vec4 dg = texture2DProj(de_distor, gl_TexCoord[0]);
	vec4 dr = dg * 0.984;
	vec4 db = dg * 1.020;
	dr[3] = dg[3] = db[3] = 1.0;

	vec4 c = vec4(0.5, 0.25, 0, 0);
	dr += c;
	dg += c;
	db += c;

	vec4 r = texture2DProj(buffer, dr);
	vec4 g = texture2DProj(buffer, dg);
	vec4 b = texture2DProj(buffer, db);

	g[2] = b[2];
	g[0] = r[0];

	gl_FragColor = g;
}
