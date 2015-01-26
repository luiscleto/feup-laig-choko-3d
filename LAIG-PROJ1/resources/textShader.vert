uniform float movTime;
uniform sampler2D secondImage;

void main() 
{
	vec4 offset=vec4(0.0,0.0,0.0,0.0);
	float maxHeight = 1.0;
	float maxShift = 0.5;
	
	
	
	vec3 greyScale = texture2D(secondImage, vec2(0,movTime)+gl_MultiTexCoord0.st).rgb / vec3(1.0, 1.0, 1.0);
	float scale = (greyScale.x + greyScale.y + greyScale.z) / 3.0;
	
	offset.y=scale*maxHeight;
	offset.z=scale*maxShift;
 
	gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex+offset);

	// pass texture coordinates from VS to FS.
	// "gl_MultiTexCoord0" has the texture coordinates assigned to this vertex in the first set of coordinates.
	// This index has to do with the set of texture COORDINATES, it is NOT RELATED to the texture UNIT.
	// "gl_TexCoord[0]" is a built-in varying that will be interpolated in the FS.
	gl_TexCoord[0] = gl_MultiTexCoord0;
}