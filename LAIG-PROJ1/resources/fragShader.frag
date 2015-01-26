uniform sampler2D baseImage;
uniform sampler2D secondImage;
uniform float movTime;

void main()
{
	vec4 color=texture2D(baseImage, vec2(0.0,movTime)+gl_TexCoord[0].st);

	// notice the coordinate conversion to flip the image horizontally and vertically
	//vec4 filter=texture2D(secondImage, vec2(0.0,movTime)-gl_TexCoord[0].st);

	//if (filter.b > 0.5)
	//color=vec4(fract(movTime),fract(movTime),fract(movTime),1.0);
	
	gl_FragColor = color; 
}
