#version 420
in vec3 color;		
//uniform vec3 triColour;
	
uniform float newRed;
uniform float newGreen;
uniform float newBlue;	
	 
void main()
{
	gl_FragColor = vec4(color, 1.0f);
//    gl_FragColor = vec4(triColour, 1.0);
	gl_FragColor = vec4(newRed, 
	                    newGreen, 
						newBlue, 1.0);
}